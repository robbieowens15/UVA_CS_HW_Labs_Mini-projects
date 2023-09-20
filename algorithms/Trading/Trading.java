/*
While all the code here is my own, I worked with Mac McLean and Christopher Osborne
on this assignment.

On my honor as a student I have neither given nor received unauthorized help on this homework:
Robert Owens
*/

import java.util.Scanner;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.Vector;
import java.util.List;

public class Trading {

    private static double walkThroughStrip(List<Point> stripPoints, double delta){

        Collections.sort(stripPoints, new yCordComparator());

        for (int i = 0; i < stripPoints.size()-1; i++){
            for (int j = i+1; j < stripPoints.size(); j++){
                double dist = Point.distBetweenTwoPoints(stripPoints.get(i), stripPoints.get(j));

                if (stripPoints.get(j).getY() - stripPoints.get(i).getY() >= delta){
                    break;
                }
                else if (dist < delta){
                    delta = dist;
                }
            }
        }
        return delta;
    }

    private static double gracefulForce(Point[] xSorted) {
        if (xSorted.length < 100){
            return bruteForce(xSorted);
        }

        Point[] left = Arrays.copyOfRange(xSorted, 0, xSorted.length/2);
        Point[] right = Arrays.copyOfRange(xSorted, xSorted.length/2, xSorted.length);

        double leftDist = gracefulForce(left);
        double rightDist = gracefulForce(right);

        double delta = Math.min(leftDist, rightDist);

        //Find the median X coordinate
        double medianXCord = 0.0;
        if (xSorted.length % 2 == 0){
            medianXCord = (((xSorted[xSorted.length/2].getX())) + (xSorted[xSorted.length/2 -1].getX()))/2;
        }
        else {
            medianXCord = xSorted[xSorted.length/2].getX();
        }


        List<Point> stripPoints = new Vector<Point>();

        for (Point p : xSorted){
            if (p.getX() > medianXCord-delta
                    && p.getX() < medianXCord+delta){
                        stripPoints.add(p);
            }
        }

        return walkThroughStrip(stripPoints, delta);
    }

    private static double bruteForce(Point[] allPoints){
        double minDistance = Double.MAX_VALUE;

        for (Point p1 : allPoints){
            for (Point p2 : allPoints){
                if (p1.equals(p2)){
                    continue;
                }
                double dist = Point.distBetweenTwoPoints(p1, p2);
                if (dist < minDistance){
                    minDistance = dist;
                }
            }
        }
        
        return minDistance;
    }

    public static void main(String args[]){
        
        Scanner sc = new Scanner(System.in);
        int numPoints = 0;

        while (true){
            numPoints = sc.nextInt();

            if (numPoints == 0){
                sc.close();
                System.exit(0);
            }

            Point[] xSorted = new Point[numPoints];

            //Create all points
            for (int i = numPoints; i > 0; i--){
                
                double x = sc.nextDouble();
                double y = sc.nextDouble();
                double z = sc.nextDouble();

                Point newPoint = new Point(x, y, z);
                
                xSorted[i-1] = newPoint;
            }

            //Sort the lists
            Arrays.sort(xSorted, new xCordComparator());

            //Compute smallest Distance
            System.out.println(covertOutput(gracefulForce(xSorted)));
        }
    }

    private static String covertOutput(double answer){
        if (answer < 10000){
            return String.format("%.4f", answer);
        }
        else {
            return "infinity";
        }
    }

    public static class xCordComparator implements Comparator<Point> {
        @Override
        public int compare(Point p1, Point p2) {
            double diff = p2.getX() - p1.getX();
            if (Math.abs(diff) < 0.0001){
                return 0;
            }
            else if (diff > 0){
                return 1;
            }
            else{
                return -1;
            }
        }
    }

    static class yCordComparator implements Comparator<Point> {

        @Override
        public int compare(Point p1, Point p2) {
            double diff = p2.getY() - p1.getY();
            if (Math.abs(diff) < 0.0001){
                return 0;
            }
            else if (diff > 0){
                return 1;
            }
            else{
                return -1;
            }
        }
    }
}