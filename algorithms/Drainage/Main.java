/*
While all the code here is my own, I worked with
Mac McLean and Christopher Osborne
on this assignment.

On my honor as a student I have neither given nor
received unauthorized help on this homework:
Robert Owens
4/22/2021
*/

import java.util.Scanner;

public class Main {

    private static int computeRun(int[][] table, int[][] cache, int i, int j){
        if (cache[i][j] != 0){
            return cache[i][j];
        }
        else {
            //0=up, 1=down, 2=right, 3=left
            int[] possiblePaths = new int[4];
            if(isValidCell(table, i+1, j)){
                if(table[i+1][j] < table[i][j]){
                    possiblePaths[0] = 1 + computeRun(table, cache, i+1, j);
                }
            }
            if(isValidCell(table, i-1,j)){
                if(table[i-1][j] < table[i][j]){
                    possiblePaths[1] = 1 + computeRun(table, cache, i-1, j);
                }
            }
            if(isValidCell(table, i, j+1)){
                if(table[i][j+1] < table[i][j]){
                    possiblePaths[2] = 1 + computeRun(table, cache, i, j+1);
                }
            }
            if(isValidCell(table, i, j-1)){
                if(table[i][j-1] < table[i][j]){
                    possiblePaths[3] = 1 + computeRun(table, cache, i, j-1);
                }
            }
            int maxLengthPath = 0;
            for(int k = 0; k < possiblePaths.length; k++){
                if (possiblePaths[k] > maxLengthPath){
                    maxLengthPath = possiblePaths[k];
                }
            }
            cache[i][j] = maxLengthPath;
            return maxLengthPath;
        }
    }

    private static int computeLongestRun(int[][] table){
        int max = 0;
        int[][] longestPathCached = new int[table.length][table[0].length];
        for(int row = 0; row < table.length; row++){
            for (int column = 0; column < table[0].length; column++){
                int length = computeRun(table, longestPathCached, row, column);
                if (length > max){
                    max = length;
                }
            }
        }
        return max+1; //For the starting cell itself.
    }

    private static boolean isValidCell(int[][] table, int i, int j){
        int rows = table.length;
        int columns = table[0].length;
        return (i >= 0 && i < rows) && (j >= 0 && j < columns);
    }
    public static void main(String[] args){
        Scanner scanner = new Scanner(System.in);
        int numCities = scanner.nextInt();

        for(int i = 0; i < numCities; i++){
            String cityName = scanner.next();
            int rows = scanner.nextInt();
            int columns = scanner.nextInt();
            int[][] table = new int[rows][columns];
            for (int j = 0; j < rows*columns; j++){
                int element = scanner.nextInt();
                table[j/columns][j%columns] = element;
            }
            System.out.println(cityName + ": " + computeLongestRun(table));
        }
        scanner.close();
    }
}