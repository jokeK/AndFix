package com.mik.andfix.web;

import com.mik.andfix.Replace;

/**
 * 模拟服务端
 *
 * 指定需要修复哪个类的哪个方法
 */
public class Mistake {
    //客户端的全包名 类名 和方法名
    @Replace(clazz = "com.mik.andfix.Mistake",method = "catchMistake")
    public int catchMistake(){
        int i = 1;
        int j = 10;
        return j/i;
    }
}
