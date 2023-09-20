/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */
#include <stdio.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>

#include <zephyr/logging/log.h>
	LOG_MODULE_REGISTER(recv);

#include "nrf_802154.h"

#define PSDU_MAX_SIZE (127u)
#define CHECK_PERIOD (800u)

static void pkt_hexdump(uint8_t *pkt, uint8_t length) {
  int i;
  printk("Packet: ");
	for (i=0; i<length; i++) {
		printk("%02x ", *pkt);
		pkt++;
	}
	printk("\n");
}

static int rf_setup(const struct device *dev)
{
	LOG_INF("RF setup started");
	ARG_UNUSED(dev);

	/* nrf radio driver initialization */
	nrf_802154_init();
	return 0;
}

void nrf_802154_received_raw(uint8_t *data, int8_t power, uint8_t lqi) {
	pkt_hexdump(data+1, *data - 2U); /* print packet from byte [1, len-2] */
	nrf_802154_buffer_free_raw(data);
}

// void rx_period_timer_handler(struct k_timer *timer); //first layer timer (outer)
void rx_timer_handler(struct k_timer *timer);
void sleep_timer_handler(struct k_timer *timer);
void stats_timer_handler(struct k_timer *timer);

K_TIMER_DEFINE(rx_timer, rx_timer_handler, NULL);
K_TIMER_DEFINE(sleep_timer, sleep_timer_handler, NULL);
K_TIMER_DEFINE(stats_timer, stats_timer_handler, NULL);

void rx_timer_handler(struct k_timer *timer)
{
	if(nrf_802154_receive()) {

	} else {
		LOG_ERR("driver could not enter the receive state");
	}
	k_timer_start(&sleep_timer, K_MSEC(8), K_MSEC(0)); // takes 4 milliseconds to transmit longest packet
}

void sleep_timer_handler(struct k_timer *timer)
{
	if(nrf_802154_sleep()) {

	} else {
		LOG_ERR("driver could not enter the sleep state");
	}
	k_timer_stop(&sleep_timer);
}

void stats_timer_handler(struct k_timer *timer)
{
    nrf_802154_stat_totals_t stats;
	nrf_802154_stat_totals_get(&stats);

	LOG_INF("[Receiver Stats] Listen: %llu us", stats.total_listening_time);
	LOG_INF("[Receiver Stats] Receive: %llu us", stats.total_receive_time);
	LOG_INF("[Receiver Stats] Transmit: %llu us", stats.total_transmit_time);
}

// void nrf_802154_receive_failed(nrf_802154_rx_error_t error, uint32_t id) {
// 	LOG_INF("rx failed error %u!", error);
// }

int main(void) {
	nrf_802154_channel_set(11u);
	nrf_802154_auto_ack_set(true); // changed from false to true
	LOG_DBG("channel: %u", nrf_802154_channel_get());
	
	// set the pan_id (2 bytes, little-endian)
	uint8_t pan_id[] = {0xcd, 0xab};
	nrf_802154_pan_id_set(pan_id);

	// set the extended address (8 bytes, little-endian)
	uint8_t extended_addr[] = {0x50, 0xbe, 0xca, 0xc3, 0x3c, 0x36, 0xce, 0xfb};
	nrf_802154_extended_address_set(extended_addr);

	k_timer_start(&rx_timer, K_MSEC(0), K_MSEC(CHECK_PERIOD));
	k_timer_start(&stats_timer, K_SECONDS(0), K_SECONDS(15));

	return 0;
}

SYS_INIT(rf_setup, POST_KERNEL, CONFIG_PTT_RF_INIT_PRIORITY);