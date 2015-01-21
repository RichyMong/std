class EqualsDemo {
    private String name;

    public EqualsDemo(String name) {
        this.name = name;
    }
    public boolean equals(Object obj) {
        if (obj instanceof EqualsDemo) {
            return name.equals(((EqualsDemo)obj).name);
        } else {
            return false;
        }
    }
    public static void main(String[] args) {
        EqualsDemo obj1 = new EqualsDemo("Ouou");
        EqualsDemo obj2 = new EqualsDemo("Ouou");
        EqualsDemo obj3 = obj1;
        System.out.println("obj1.equals(obj2) = " + obj1.equals(obj2));
        System.out.println("obj1 == obj2 " + (obj1 == obj2));
        System.out.println("obj1 == obj3 " + (obj1 == obj3));
    }
}
