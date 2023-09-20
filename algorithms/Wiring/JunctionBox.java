import java.util.HashSet;

public class JunctionBox extends Vertex{
    public JunctionBox(String name){
        this.name = name;
        this.edges = new HashSet<Edge>();
    }
}
