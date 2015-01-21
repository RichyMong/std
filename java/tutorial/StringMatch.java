class StringMatch {
    public static void main(String[] args) {
        String searchMe = "two eggs";
        String findMe = "eggs";
        System.out.printf("%s contains %s: %s%n", searchMe, findMe, searchMe.contains(findMe));
    }
}
