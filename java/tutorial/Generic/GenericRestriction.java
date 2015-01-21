import java.util.List;
import java.util.ArrayList;

class GenericRestriction {
    static <T> void rtti(List<T> list) {
        if (list instanceof ArrayList<?>)
            System.out.println("ArrayList!");
    }
    public static void main(String[] args) {
        ArrayList<Integer> ali = new ArrayList<>();
        rtti(ali);
    }
}
