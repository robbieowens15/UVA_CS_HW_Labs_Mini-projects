import java.util.HashSet;

public class BreakerBox extends Vertex{
    public BreakerBox(String name){
        this.name = name;
        this.edges = new HashSet<Edge>();
    }
}
