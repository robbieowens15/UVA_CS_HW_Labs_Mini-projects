import java.util.HashSet;

public class Switch extends Vertex{

    public Switch(String name){
        this.name = name;
        this.edges = new HashSet<Edge>();
    }

    @Override
    public boolean equals(Object o){
        if (o == this) return true;
        if (!(o instanceof Switch)) return false;
        Switch s = (Switch) o;
        return this.name == s.name;
    }
}
