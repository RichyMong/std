import java.util.List;
import java.util.Arrays;

class UpperBound {
    static <T extends Number> void scanList(List<T> list) {
        System.out.println(list.get(0).getClass().getSimpleName());
        for (T t : list) {
            System.out.println(t);
        }
    }
    public static void main(String[] args) {
        List<Number> aln = Arrays.asList(new Integer(1), new Double(2.0));
        List<Integer> ali = Arrays.asList(1, 2);
        List<Double> ald = Arrays.asList(1.1, 2.2);
        UpperBound.scanList(aln);
        UpperBound.scanList(ali);
        UpperBound.scanList(ald);
    }
}
