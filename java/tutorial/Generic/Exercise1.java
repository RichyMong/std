class Exercise1 {
    interface Predicate<T>  {
        boolean test(T obj);
    }
    static class Even implements Predicate<Integer> {
        public boolean test(Integer i) {
            return i % 2 == 0;
        }
    }
    static class Palindrome implements Predicate<String> {
        public boolean test(String s) {
            for (int i = 0; i < s.length(); ++i) {
                if (s.charAt(i) != s.charAt(s.length() - 1 - i))
                    return false;
            }
            return true;
        }
    }
    public static void main(String[] args) {
        Predicate<Integer> p = new Exercise1.Even();
        System.out.println(p.test(2));
        Predicate<String> ps = new Exercise1.Palindrome();
        System.out.println(ps.test("abcba"));
    }
}
