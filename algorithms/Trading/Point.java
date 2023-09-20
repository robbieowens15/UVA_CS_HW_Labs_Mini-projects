import java.util.Comparator;

public class Point {
    private double x;
    private double y;
    private double z;

    public Point(){
        this.x = 0.0;
        this.y = 0.0;
        this.z = 0.0;
    }

    public Point(double x, double y, double z){
        this.x = x;
        this.y = y;
        this.z = z;
    }

    public double getX(){
        return this.x;
    }

    public double getY(){
        return this.y;
    }

    public double getZ(){
        return this.z;
    }

    public String toString(){
        return "(" + this.x + ", " + this.y + ", " + this.z + ")";
    }

    public boolean equals(Point point){
        double epsilon = 0.00001;
        if (Math.abs(this.x - point.x) < epsilon 
            && Math.abs(this.y - point.y) < epsilon
            && Math.abs(this.z - point.z) < epsilon){
                return true;
            }
        else{
            return false;
        }
    }

    public static double distBetweenTwoPoints(Point p1, Point p2) {

        return Math.sqrt(Math.pow(p1.getX()-p2.getX(), 2) 
            + Math.pow(p1.getY() - p2.getY(), 2) 
            + Math.pow(p1.getZ() - p2.getZ(), 2));
    }
}
