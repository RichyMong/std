import java.util.Scanner;

public class MaxSquare {
    public static void printMatrix(int[][] matrix) {
        for (int i = 0; i < matrix.length; ++i) {
            for (int j = 0; j < matrix[i].length; ++j) {
                System.out.printf("%d ", matrix[i][j]);
            }
            System.out.printf("%n");
        }
    }

    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        int row = sc.nextInt();
        int col = sc.nextInt();

        int[][] numbers = new int[row][col];
        for (int i = 0; i < row; ++i) {
            for (int j = 0; j < col; ++j) {
                numbers[i][j] = sc.nextInt();
            }
        }

        printMatrix(numbers);

        int[][] result = new int[row][col];

        System.arraycopy(numbers[0], 0, result[0], 0, row);

        for (int j = 0; j < col; ++j) {
            result[0][j] = numbers[0][j];
        }

        for (int i = 1; i < row; ++i) {
            for (int j = 1; j < col; ++j) {
                if (numbers[i][j] == 0) {
                    result[i][j] = 0;
                } else {
                    result[i][j] = Math.min(Math.min(result[i][j-1], result[i-1][j]),
                                            result[i-1][j-1]) + 1;
                }
            }
        }

        printMatrix(result);

        int max_s = result[0][0], max_i = 0, max_j = 0;
        for (int i = 0; i < row; ++i) {
            for (int j = 0; j < col; ++j) {
                if (result[i][j] > max_s) {
                    max_s = result[i][j];
                    max_i = i;
                    max_j = j;
                }
            }
        }

        System.out.printf("Max square size %d at %d:%d%n", max_s, max_i, max_j);
    }
}
