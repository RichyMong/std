import java.util.Scanner;
import java.util.Arrays;

public class Exchange {
    public static void moveToTail(int[] orders, int i, int tail) {
        int tmp = orders[i];
        orders[i] = orders[0];
        orders[0] = orders[tail];
        orders[tail] = tmp;
    }

    public static int sort(int[] orders, int size) {
        if (size == 0) {
            return 0;
        }

        if (orders[size - 1] == size) {
            return sort(orders, size - 1);
        } else if (orders[0] == size) {
            moveToTail(orders, 0, size - 1);
            return sort(orders, size - 1) + 1;
        } else {
            for (int i = 1; i < size; ++i) {
                if (orders[i] == size) {
                    moveToTail(orders, i, size - 1);
                    System.out.println(Arrays.toString(orders));
                    return sort(orders, size - 1) + 2;
                }
            }
        }
        return 0;
    }

    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);

        int count = sc.nextInt();
        int[] orders = new int[count];
        for (int i = 0; i < orders.length; ++i) {
            orders[i] = sc.nextInt();
        }

        System.out.println(sort(orders, count));
        System.out.println(Arrays.toString(orders));
    }
}
