import java.util.HashSet;

public class Light extends Vertex{
    public Switch behindSwitch;

    public Light(String name, Switch s){
        this.name = name;
        this.edges = new HashSet<Edge>();
        behindSwitch = s;
    }
}
