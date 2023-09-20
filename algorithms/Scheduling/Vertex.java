import java.util.HashSet;

public abstract class Vertex {
    public String name;
    public HashSet<Edge> outEdges;

    public Vertex(String name){
        this.name = name;
        outEdges = new HashSet<Edge>();
    }

    public boolean hasEdgeTo(Vertex vertex){
        for (Edge edge : outEdges) {
            if(edge.v.equals(vertex)){
                return true;
            }
        }
        return false;
    }

    @Override
    public String toString() {
        return name.toString();
    }

    @Override
    public int hashCode() {
        return name.hashCode();
    }

    @Override 
    public boolean equals(Object o){
        if (o == null) return false;
        if (!(o instanceof Vertex)) return false;
        return name.equals(((Vertex)o).name);
    }
}
