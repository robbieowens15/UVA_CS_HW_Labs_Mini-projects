import java.util.HashSet;
import java.util.HashMap;

public class DisjointSet {

    /*
    Idea: Implement the Disjoint set just like it is for n integer, 
    but have a HashMap in the back doing all the conversions
    */

    private int[] array;
    private int[] rank;
    private HashMap<Integer, Vertex> forward;
    private HashMap<Vertex, Integer> backward;

    public DisjointSet(HashSet<Vertex> vertices){
        //MakeSet. should make independent Sets
        array = new int[vertices.size()];
        rank = new int[vertices.size()];
        forward = new HashMap<Integer, Vertex>();
        backward = new HashMap<Vertex, Integer>();
        int i = 0; 
        for(Vertex v : vertices){
            array[i] = i;
            rank[i] = 0;
            forward.put(i, v);
            backward.put(v, i);
            i++;
        }
    }

    public Vertex findSet(Vertex v){
        int i = backward.get(v);
        if (i == array[i]){
            return forward.get(i);
        }
        else{
            int belongsTo = backward.get(this.findSet(forward.get(array[i])));
            array[i] = belongsTo;
            return forward.get(belongsTo);
        }
    }

    public void unionSets(Vertex v1, Vertex v2){
        int iSet = backward.get(findSet(v1));
        int jSet = backward.get(findSet(v2));

        if (iSet == jSet) return;

        int iRank = rank[iSet];
        int jRank = rank[jSet];

        if (iRank < jRank){
            this.array[iSet] = jSet;
        }
        else if (iRank > jRank){
            this.array[jSet] = iSet;
        }
        else{
            this.array[iSet] = jSet;
            rank[jSet]++;
        }
        return;
    }

    public boolean equivalentSets(Vertex v1, Vertex v2){
        return backward.get(findSet(v1)) == backward.get(findSet(v2));
    }
}
