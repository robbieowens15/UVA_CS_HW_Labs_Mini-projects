#include <stdio.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
	LOG_MODULE_REGISTER(send);

#include "nrf_802154.h"
#include <inttypes.h>

#define PSDU_MAX_SIZE (127u)
#define FCS_LENGTH (2u) /* Length of the Frame Control Sequence */
#define CHECK_PERIOD (800u) //500 microseconds

#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>


void check_period_timer_handler(struct k_timer *timer);
void stats_timer_handler(struct k_timer *timer);

K_TIMER_DEFINE(check_period_timer, check_period_timer_handler, NULL);
K_TIMER_DEFINE(stats_timer, stats_timer_handler, NULL);

void check_period_timer_handler(struct k_timer *timer)
{
	k_timer_stop(&check_period_timer);
	printk("Timer has expired.");
}

void stats_timer_handler(struct k_timer *timer){
	nrf_802154_stat_totals_t stats;
	nrf_802154_stat_totals_get(&stats);

	LOG_INF("\nStatistics");
	LOG_INF("[Receiver Stats] Listen: %llu us", stats.total_listening_time);
	LOG_INF("[Receiver Stats] Receive: %llu us", stats.total_receive_time);
	LOG_INF("[Receiver Stats] Transmit: %llu us", stats.total_transmit_time);
	LOG_INF("\n");
}


#define SW0_NODE	DT_ALIAS(sw0)

static const struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});

static struct gpio_callback button1_cb_data;

bool waiting_for_ack = true;
void start_packet_train(void){
	// set the pan_id (2 bytes, little-endian)
	uint8_t src_pan_id[] = {0xcd, 0xab}; 
	nrf_802154_pan_id_set(src_pan_id);

	// set the extended address (8 bytes, little-endian)
	uint8_t src_extended_addr[] = {0xdc, 0xa9, 0x35, 0x7b, 0x73, 0x36, 0xce, 0xf4};
	nrf_802154_extended_address_set(src_extended_addr);

	uint8_t dst_extended_addr[] = {0x50, 0xbe, 0xca, 0xc3, 0x3c, 0x36, 0xce, 0xfb};

	// send a tx pkt
	uint8_t pkt[PSDU_MAX_SIZE];
	pkt[0] = 32u + FCS_LENGTH; /* Length for nrf_transmit (length of pkt + FCS) */
	pkt[1] = 0x21; /* Frame Control Field */
	pkt[2] = 0xcc; /* Frame Control Field */
	pkt[3] = 0x00; /* Sequence number */
	pkt[4] = 0xff; /* Destination PAN ID 0xffff */
	pkt[5] = 0xff; /* Destination PAN ID */
	memcpy(&pkt[6], dst_extended_addr, 8); /* Destination extended address */
	memcpy(&pkt[14], src_pan_id, 2); /* Source PAN ID */
	memcpy(&pkt[16], src_extended_addr, 8);/* Source extended address */ 
	pkt[24] = 'G'; /* Payload */ 
	pkt[25] = 'r'; /* */
	pkt[26] = 'o'; /* */
	pkt[27] = 'u';
	pkt[28] = 'p';
	pkt[29] = ' ';
	pkt[30] = '1';
	pkt[31] = '1';

	const nrf_802154_transmit_metadata_t metadata = {
			.frame_props = NRF_802154_TRANSMITTED_FRAME_PROPS_DEFAULT_INIT,
			.cca = true
		};
	printk("started packet train\n");
	k_timer_start(&check_period_timer, K_MSEC(CHECK_PERIOD), K_MSEC(CHECK_PERIOD));
	waiting_for_ack = true;
	while((k_timer_remaining_get(&check_period_timer) > 0) && waiting_for_ack == true) {
		// send the packet
		printk("packet transmitted");
		if(!nrf_802154_transmit_raw(pkt, &metadata)) { 
			//LOG_ERR("driver could not schedule the transmission procedure.");
		}
	}

	if (waiting_for_ack == false) {
		LOG_ERR("Packet Train Ended with %d ms remaining - ack received\n", k_timer_remaining_get(&check_period_timer));
	}

}


void button1_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	int val = gpio_pin_get_dt(&button1);
	if (val == 1) {
		start_packet_train();
		printk("Button1 pressed.\n");
	}
}


void init_button(const struct gpio_dt_spec* button,
                 struct gpio_callback* button_cb_data,
				 gpio_callback_handler_t cb) {
	int ret;

	if (!device_is_ready(button->port)) {
		printk("Error: button device %s is not ready\n",
		       button->port->name);
		return;
	}

	ret = gpio_pin_configure_dt(button, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, button->port->name, button->pin);
		return;
	}

	ret = gpio_pin_interrupt_configure_dt(button, GPIO_INT_EDGE_BOTH);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, button->port->name, button->pin);
		return;
	}

	gpio_init_callback(button_cb_data, cb, BIT(button->pin));
	gpio_add_callback(button->port, button_cb_data);
	printk("Set up button at %s pin %d\n", button->port->name, button->pin);
}


static int rf_setup(const struct device *dev)
{
	LOG_INF("RF setup started");
	ARG_UNUSED(dev);

	/* nrf radio driver initialization */
	nrf_802154_init();

	return 0;
}

// callback fn when tx starts
void nrf_802154_tx_started(const uint8_t *p_frame) {
	LOG_INF("tx started");
}

// callback fn when tx fails
void nrf_802154_transmit_failed(uint8_t *frame, nrf_802154_tx_error_t error,
	const nrf_802154_transmit_done_metadata_t *p_metadata)
{
	LOG_INF("tx failed error - a.k.a No ACK Received. %u!", error);
}

// callback fn for successful tx
void nrf_802154_transmitted_raw(uint8_t *p_frame, 
	const nrf_802154_transmit_done_metadata_t *p_metadata) {
	LOG_INF("frame was transmitted! - yes the ACK was received!");
	// Stop the transmission train
	waiting_for_ack = false;
}

int main(int argc, char **argv) {
	LOG_DBG("BOOTING...");
	k_timer_start(&stats_timer, K_SECONDS(0), K_SECONDS(15));

	//? nrf_802154_ack_timeout_set(1); //10ms after transmission, say acknowledgement failed

	init_button(&button1, &button1_cb_data, button1_pressed);
	nrf_802154_channel_set(11u);
	LOG_DBG("channel: %u", nrf_802154_channel_get());
	return 0;
}

SYS_INIT(rf_setup, POST_KERNEL, CONFIG_PTT_RF_INIT_PRIORITY);