import java.util.ArrayList;

class WildCard {
    <T> void runGeneric(T t) {
        System.out.println("Generic: " + t);
    }

    <T> void runGeneric(ArrayList<T> at) {
        for (Object t : at) {
            System.out.println(t);
        }
    }

    void runWildCard(ArrayList<? extends Object> at) {
        for (Object t : at) {
            System.out.println(t);
        }
    }

    public static void main(String[] args) {
        WildCard wc = new WildCard();
        wc.runGeneric("abc");
        ArrayList<Integer> ai = new ArrayList<>();
        ai.add(1);
        ai.add(2);
        ai.add(3);
        wc.runGeneric(ai);
        wc.runWildCard(ai);
    }
}
