/*
While all the code here is my own, I worked with
Mac McLean and Christopher Osborne
on this assignment.

On my honor as a student I have neither given nor
received unauthorized help on this homework:
Robert Owens
5/2/2021
*/

import java.util.Scanner;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Queue;
import java.util.Stack;

public class Main {
    public static void main(String args[]) {
        Scanner scanner = new Scanner(System.in);
        while(true) {
            int numRegistrationsRequests = scanner.nextInt();
            int numCourses = scanner.nextInt();
            int maxCoursesPerStudent = scanner.nextInt();
            scanner.nextLine();
            Student.limit = maxCoursesPerStudent;

            if (numRegistrationsRequests == numCourses
            && numCourses == maxCoursesPerStudent
            && numCourses == 0) {
                break;
            }
            HashMap<Integer, Vertex> valToVertex = new HashMap<Integer, Vertex>();
            HashMap<Vertex, Integer> vertexToVal = new HashMap<Vertex, Integer>();
            int counter = 0;

            Vertex s = new Source();
            valToVertex.put(counter, s);
            vertexToVal.put(s, counter);
            counter++;

            Vertex t = new Sink();

            int numStudents = 0;
            HashMap<String, Student> students = new HashMap<String,Student>();
            HashMap<String, Course> courses = new HashMap<String, Course>();

            for (int i = 0; i < numRegistrationsRequests; i++) {
                String studentName = scanner.next();
                String courseName = scanner.next();
                scanner.nextLine();

                Student curStudent = null;
                Course curCourse = null;
                //Student exists
                if (students.containsKey(studentName)){
                    curStudent = students.get(studentName);
                }
                //Student does not exist yet, create them
                else{
                    curStudent = new Student(studentName);
                    students.put(studentName, curStudent);
                    valToVertex.put(counter, curStudent);
                    vertexToVal.put(curStudent, counter);
                    counter++;
                    numStudents++;
                }
                //Course exists
                if (courses.containsKey(courseName)){
                    curCourse = courses.get(courseName);
                }
                //Course does not yet exist, create it
                else {
                    curCourse = new Course(courseName);
                    courses.put(courseName, curCourse);
                }
                curStudent.outEdges.add(new Edge(curStudent, curCourse, 1));
            }

            for (Vertex student : students.values()) {
                s.outEdges.add(new Edge(s, student, Student.limit, Student.limit));
            }

            for(int i = 0; i < numCourses; i++){
                String courseName = scanner.next();
                int courseCapacity = scanner.nextInt();
                scanner.nextLine();

                Course curCourse = courses.get(courseName);
                curCourse.outEdges.add(new Edge(curCourse, t, curCourse.limit));
                valToVertex.put(counter, curCourse);
                vertexToVal.put(curCourse, counter);
                counter++;
                curCourse.limit = courseCapacity;
            }

            valToVertex.put(counter, t);
            vertexToVal.put(t, counter);
            counter++;

            int[][] residuals = new int[counter][counter];
            for(int i = 0; i < counter; i++){
                for(int j = 0; j < counter; j++){
                    Vertex v1 = valToVertex.get(i);
                    Vertex v2 = valToVertex.get(j);

                    if (v1.hasEdgeTo(v2)){
                        if (v1 instanceof Source){
                            residuals[i][j] = Student.limit;
                        }
                        else if (v1 instanceof Student){
                            residuals[i][j] = 1;
                        }
                        else if (v1 instanceof Course){
                            residuals[i][j] = ((Course) v1).limit;
                        }
                    }
                    else{
                        residuals[i][j] = 0;
                    }
                }
            }

            int[] path = new int[counter];

             if(fordFulkerson(counter, residuals, path, vertexToVal.get(s), vertexToVal.get(t)) == numStudents*Student.limit){
                 System.out.println("Yes");
             }
             else{
                 System.out.println("No");
             }
        }
        scanner.close();
    }

    private static boolean bfs(int numVertices, int[][] residuals, int[] path, int s, int t){
        Boolean[] visited = new Boolean[numVertices];
        for(int i = 0; i < numVertices; i++){
            visited[i] = false;
        }
        Queue<Integer> queue = new LinkedList<Integer>();
        queue.add(s);
        visited[s] = true;
        path[s] = -1;
        while(! queue.isEmpty()){
            int i = queue.poll();
            for(int j = 0; j < visited.length; j++){
                if (!visited[j] && residuals[i][j] > 0){
                    if (j == t){
                        path[j] = i;
                        return true;
                    }
                    queue.add(j);
                    path[j] = i;
                    visited[j] = true;
                }
            }
        }
        return false;
    }

    private static int fordFulkerson(int numVertices, int[][] residuals, int[] path, int s, int t){
        int maxFlow = 0;
        while(bfs(numVertices, residuals, path, s, t)){
            int flow = Integer.MAX_VALUE;
            for(int j = t; j != s; j = path[j]){
                int i = path[j];
                flow = Math.min(flow, residuals[i][j]);
            }
            for(int j = t; j != s; j = path[j]){
                int i = path[j];
                residuals[i][j] -= flow;
                residuals[j][i] += flow;
            }
            maxFlow += flow;
        }
        return maxFlow;
    }
}