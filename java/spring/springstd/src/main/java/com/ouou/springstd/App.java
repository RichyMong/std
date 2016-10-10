package com.ouou.springstd;

import org.springframework.context.ApplicationContext;
import org.springframework.context.support.ClassPathXmlApplicationContext;

public class App {
	public static void main(String[] args) {
		ApplicationContext context = new ClassPathXmlApplicationContext("com/ouou/springstd/HelloBeans.xml");
		try {
			MessageBean obj = (MessageBean) context.getBean("helloBean");
			System.out.println(obj.getMessage());
		} finally {
			((ClassPathXmlApplicationContext)context).close();
		}

	}
}