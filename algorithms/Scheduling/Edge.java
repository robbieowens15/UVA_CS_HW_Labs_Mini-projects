public class Edge {
    //Directed Edge from u to v
    public Vertex u;
    public Vertex v;
    public int flow;
    public int capacity;
    public int lowerBound;

    public Edge(Vertex u, Vertex v, int c, int l, int f){
        this.u = u;
        this.v = v;
        flow = f;
        capacity = c;
        lowerBound = 0;
    }

    public Edge(Vertex u, Vertex v, int c){
        this.u = u;
        this.v = v;
        flow = 0;
        capacity = c;
        lowerBound = 0;
    }

    public Edge(Vertex u, Vertex v, int c, int l){
        this.u = u;
        this.v = v;
        flow = 0;
        capacity = c;
        lowerBound = l;
    }

    @Override
    public String toString() {
        return u.name + " -> " + v.name;
    }
}
