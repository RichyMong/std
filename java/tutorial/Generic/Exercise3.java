class Exercise3 {
    static <T> void exchange(T[] objs, int src, int dst) {
        T temp = objs[src];
        objs[src] = objs[dst];
        objs[dst] = temp;
        for (T obj : objs) {
            System.out.print(obj + " ");
        }
        System.out.print("\n");
    }
    public static void main(String[] args) {
        Integer[] ints = {1, 2, 3};
        exchange(ints, 0, 2);
        String[] strs = {"hi", "hey", "hello"};
        exchange(strs, 0, 2);
    }
}
