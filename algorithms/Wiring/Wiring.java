/*
While all the code here is my own, I worked with 
Mac McLean and Christopher Osborne
on this assignment.

On my honor as a student I have neither given nor 
received unauthorized help on this homework:
Robert Owens
3/28/3021
*/

/*
!Notes on typ electrical inputs:
Breaker Box: This is a special node in from which
the electricity is sourced. There will only be one
breaker box that supplies electricity to your entire
house.

Switch: A switch is a special node that can affect
the current moving along a path from the breaker
box, and can be turned off to not allow current to
continue past the switch onward.

Light: A light is a node that is controlled by a switch.
Thus, there MUST be exactly one switch between the light and the breaker box.
Each light has a specific switch you want to control it with, and thus that 
particular switch must be the one between the light and breaker box.

Electrical Outlet: You want your outlets to be active all of the time. 
Thus, an outlet must have a path directly to the breaker box with NO switches in
between.

Junction Box: A junction box is simply a location
that can connect multiply wires together. 
For example, one wire from the breaker can go into a junction box, and 
three wires can fan out of the box to distribute electricity to the house.
*/

/*
!Notes from Floran's overview of this homework.
Junctions are vertices
Connections are edges

Goal is to create a min spanning tree
Note that some edges that are provided in input may not be valid!

Multiple lights can exist after a single switch iff they have the same
'behind' switch
Only lights can exist behind switches
Lights can exist in series or in parallel 'behind' a switch
Cannot connect lights that should exist behind two unique switches
Outlets can be directly connected to a switch
Two switches cannot be connected or on the same path from breaker

Breaker to light is NOT allowed
Junction to light is NOT allowed

Note for this problem junctions and outlets are essentially the same

When reading input if a switch exists, then the lights MUST 'run behind'
that switch
It may  not always be the case that junction names are indicative of their type
    i.e. j1 could be a light

No in-out node requirements
Wries are undirected edges (so check both orderings of edge)

Step #1:
When reading input do not add invalid edges to the graph. Ignore them
Step $2:
Validating that edges from outlets/junctions to switches
Step #3:
Note: Switches can only have 1 non light connection
Run MST on all breaker, outlet, junction nodes (Forest 1)
Run MST on all switches and lights (Forest 2)
Run MST across Forest 1 and Forest 2 to finish up the problem
*/
import java.util.Scanner;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Queue;
import java.util.PriorityQueue;
import java.util.ArrayDeque;
import java.util.Comparator;

public class Wiring {

    private static HashSet<Edge> mstEdges = new HashSet<Edge>();

    private static void kruskalMST(PriorityQueue<Edge> edges, HashSet<Vertex> vertices){
        int edgesAccepted = 0;
        DisjointSet disjointSet = new DisjointSet(vertices);

        while((edgesAccepted < vertices.size()-1) && edges.size() != 0){
            //Min cost edge (u,v)
            Edge e = edges.poll();
            Vertex uSet = disjointSet.findSet(e.v1);
            Vertex vSet = disjointSet.findSet(e.v2);
            if (!(disjointSet.equivalentSets(uSet, vSet))){
                edgesAccepted++;
                disjointSet.unionSets(uSet, vSet);
                //Create the edge between uSet and vSet
                uSet.addEdge(e);
                vSet.addEdge(e);
                mstEdges.add(e);
            }
        }
    }
    public static void main(String[] args){
        //Read Input
        Scanner sc = new Scanner(System.in);
        int numJunctions = sc.nextInt();
        int numConnections = sc.nextInt();

        HashMap<String, Vertex> labelToVertexMap
            = new HashMap<String, Vertex>();

        HashSet<Vertex> vertices = new HashSet<Vertex>();
        HashSet<Vertex> noSwitchDependencies = new HashSet<Vertex>();
        HashSet<Vertex> lightsAndSwitches = new HashSet<Vertex>();

        Switch prevSwitch = null;
        int numSwitch = 0;
        for (int i = 0; i < numJunctions; i++){
            String name = sc.next();
            String type = sc.next();
            sc.nextLine();
            Vertex newVertex;
            switch (type){
                case "breaker": 
                    newVertex = new BreakerBox(name);
                    noSwitchDependencies.add(newVertex);
                    break;
                case "box":
                    newVertex = new JunctionBox(name);
                    noSwitchDependencies.add(newVertex);
                    break;
                case "switch":
                    newVertex = new Switch(name);
                    prevSwitch = (Switch) newVertex;
                    lightsAndSwitches.add(newVertex);
                    numSwitch++;
                    break;
                case "light":
                    newVertex = new Light(name, prevSwitch);
                    lightsAndSwitches.add(newVertex);
                    break;
                case "outlet":
                    newVertex = new ElectricalOutlet(name);
                    noSwitchDependencies.add(newVertex);
                    break;
                default:
                    System.out.println("Something has gone horribly wrong");
                    newVertex = null;
                    break;
            }
            labelToVertexMap.put(name, newVertex);
            vertices.add(newVertex);
        }

        PriorityQueue<Edge> directlyFromBreaker = 
            new PriorityQueue<Edge>(new EdgeComparator());
        PriorityQueue<Edge> involvesSwitch = 
            new PriorityQueue<Edge>(new EdgeComparator());
        PriorityQueue<Edge> crossEdges = 
            new PriorityQueue<Edge>(new EdgeComparator());

        for(int i = 0; i < numConnections; i++){
            String v1Name = sc.next();
            String v2Name = sc.next();
            int cost = sc.nextInt();
            sc.nextLine();

            //Get vertex objects
            Vertex v1 = labelToVertexMap.get(v1Name);
            Vertex v2 = labelToVertexMap.get(v2Name);
            
            //Check Validity of edge
            if (Edge.isValidEdge(v1, v2)){
                Edge newEdge = new Edge(v1, v2, cost);
                //Switch-light or light-light edge
                if ((v1 instanceof Switch && v2 instanceof Light) ||
                    (v1 instanceof Light && v2 instanceof Switch) ||
                    (v1 instanceof Light && v2 instanceof Light)){
                        involvesSwitch.add(newEdge);
                    }
                //Switch to anything but light 
                else if (v1 instanceof Switch || v2 instanceof Switch){
                    crossEdges.add(newEdge);
                }
                //Directly from breaker but not involving a switch
                else
                    directlyFromBreaker.add(newEdge);
            }
        }
        sc.close();
        //Done Reading Input
        kruskalMST(directlyFromBreaker, noSwitchDependencies);
        kruskalMST(involvesSwitch, lightsAndSwitches);
        //Connect the non switch MST to the switch MST
        //By making a connection for every switch
        HashSet<Switch> hookedInSwitches = new HashSet<Switch>();
        while (hookedInSwitches.size() != numSwitch){
            //Edge minEdge (u, v)
            Edge minEdge = crossEdges.poll();
            Vertex u = minEdge.v1;
            Vertex v = minEdge.v2;

            Switch theSwitch = null;
            Vertex nonSwitch = null;

            if (u instanceof Switch){
                //u is the switch
                theSwitch = (Switch) u;
                nonSwitch = v;
            }
            else{
                //v is the switch
                theSwitch = (Switch) v;
                nonSwitch = u;
            }

            if (hookedInSwitches.contains(theSwitch)) continue;
            else{
                theSwitch.addEdge(minEdge);
                nonSwitch.addEdge(minEdge);
                hookedInSwitches.add(theSwitch);
                mstEdges.add(minEdge);
            }
        }
        //MST is complete!!
        //Now Calculate cost of MST
        int sum = 0;
        for(Edge e : mstEdges){
            sum += e.cost;
        }
        System.out.println(sum);
    }

    private static int calcTotalCost(Vertex source){
        int sumCost = 0;
        Queue<Vertex> queue = new ArrayDeque<Vertex>();
        HashSet<Vertex> visited = new HashSet<Vertex>();
        HashSet<Edge> seenEdge = new HashSet<Edge>();
        queue.offer(source);
        while(queue.size() != 0){
            Vertex curVertex = queue.poll();
            visited.add(curVertex);
            for(Edge e : curVertex.edges){
                if (!seenEdge.contains(e)){
                    sumCost += e.cost;
                    seenEdge.add(e);
                }
                //e (u, v)
                Vertex u = e.v1;
                Vertex v = e.v2;
                if (u.equals(curVertex)){
                    //v needs to be processed
                    if(!visited.contains(v)) queue.offer(v);
                } 
                else{
                    //u needs to be processed
                    if(!visited.contains(u)) queue.offer(u);
                }
            }
        }
        return sumCost;
    }

    static class EdgeComparator implements Comparator<Edge> {
        @Override
        public int compare(Edge e1, Edge e2){
            return Integer.compare(e1.cost, e2.cost);
        }
    }
}