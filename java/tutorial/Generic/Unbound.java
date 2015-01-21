import java.util.Arrays;
import java.util.List;
import java.util.ArrayList;

class Unbound {
    static <T> void unbound(List<T> list) {
        System.out.println(list.getClass().getSimpleName());
        for (T t : list) {
            System.out.print(t + " ");
        }
        System.out.println("");
    }
    static void unbound2(List<? extends Object> list) {
        System.out.println(list.getClass().getSimpleName());
        for (Object t : list) {
            System.out.print(t + " ");
        }
        System.out.println("");
    }
    static void unbound3(List<?> list) {
        System.out.println(list.getClass().getSimpleName());
        for (Object t : list) {
            System.out.print(t + " ");
        }
        System.out.println("");
    }
    public static void main(String[] args) {
        List<Integer> li = Arrays.asList(1, 2, 3);
        unbound(li);
        unbound2(li);
        unbound3(li);
        List<Object> lx = new ArrayList<>();
        lx.add(new Integer(1));
        lx.add(new Float(1.0f));
        unbound(lx);
        unbound2(lx);
        unbound3(lx);
    }
}
