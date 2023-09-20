import java.util.HashSet;

public abstract class Vertex {
    public String name;
    public HashSet<Edge> edges;

    public boolean equals(Object o){
        if (o == this) return true;
        if (!(o instanceof Vertex)) return false;
        Vertex v1 = (Vertex) o;
        return v1.name.equals(this.name);
    }

    public void addEdge(Edge e){
        edges.add(e);
    }
}
