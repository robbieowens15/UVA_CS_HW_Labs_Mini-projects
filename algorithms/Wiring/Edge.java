public class Edge {
    public Vertex v1;
    public Vertex v2;
    public int cost;

    public Edge(Vertex v1, Vertex v2, int cost){
        this.v1 = v1;
        this.v2 = v2;
        this.cost = cost;
    }

    public static boolean isValidEdge(Vertex v1, Vertex v2){
        //Invalid edges. Create paths that shouldn't be allowed. 
        //Lights exist behind switches
        if ((v1 instanceof Light && v2 instanceof BreakerBox) ||
            (v1 instanceof BreakerBox && v2 instanceof Light) ||
            (v1 instanceof Light && v2 instanceof JunctionBox)||
            (v1 instanceof JunctionBox && v2 instanceof Light)||
            (v1 instanceof Light && v2 instanceof ElectricalOutlet)||
            (v1 instanceof ElectricalOutlet && v2 instanceof Light)||
            (v1 instanceof Switch && v2 instanceof Switch))
                return false;
        //Lights can connect to other lights iff they share the same back switch
        else if(v1 instanceof Light && v2 instanceof Light){
            Light l1 = (Light) v1;
            Light l2 = (Light) v2;
            return l1.behindSwitch.equals(l2.behindSwitch);
        }
        else if(v1 instanceof Light && v2 instanceof Switch){
            Light l = (Light) v1;
            Switch s = (Switch) v2;
            return l.behindSwitch.equals(s);
        }
        else if(v1 instanceof Switch && v2 instanceof Light){
            Light l = (Light) v2;
            Switch s = (Switch) v1;
            return l.behindSwitch.equals(s);
        }
        else
            return true;
    }

    public boolean contains(Vertex v1){
        return (v1.equals(this.v1) || v1.equals(this.v2));
    }

    public String toString(){
        return "(" + v1.toString() + ", " + v2.toString() + ")";
    }
}
