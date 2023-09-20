import java.util.HashSet;

public class ElectricalOutlet extends Vertex{
    public ElectricalOutlet(String name){
        this.name = name;
        this.edges = new HashSet<Edge>();
    }
}
