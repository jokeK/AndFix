package com.mik.andfix;

import android.content.Context;
import android.os.Build;

import java.io.File;
import java.io.IOException;
import java.lang.reflect.Method;
import java.util.Enumeration;

import dalvik.system.DexFile;

public class DexManager {
    private Context context;
    private static final DexManager ourInstance = new DexManager();

    public static DexManager getInstance() {
        return ourInstance;
    }

    private DexManager() {
    }

    public void setContext(Context context) {
        this.context = context;
    }

    public void loadFile(File file) {
        try {
            //加载修复好的dex文件 参数二缓存路径
            DexFile dexFile = DexFile.loadDex(file.getAbsolutePath(),
                    new File(context.getCacheDir(), "opt").getAbsolutePath(),
                    Context.MODE_PRIVATE);
            //得到class
            Enumeration<String> entries = dexFile.entries();
            while (entries.hasMoreElements()) {
                //拿到全类名
                String className = entries.nextElement();
                Class clazz = dexFile.loadClass(className, context.getClassLoader());
                if (clazz != null) {
                    fixClass(clazz);
                }
            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void fixClass(Class clazz) {
        //得到修复好的方法
        Method[] methods = clazz.getDeclaredMethods();
        for (Method rightMethod : methods){
            Replace replace = rightMethod.getAnnotation(Replace.class);
            if (replace == null){
                continue;
            }
            //已经找到了修复好的方法  下面找到需要修复的方法
            //通过修复好的class的注解得到错误方法的类名和方法名
            String wrongClassName = replace.clazz();
            String wrongMethodName = replace.method();
            try {
                Class cls = Class.forName(wrongClassName);
                //得到错误的方法第一个参数方法名
                //第二个参数方法的参数类型，通过修复好的方法的参数类型来确定错误的方法参数类型
                Method wrongMethod = cls.getDeclaredMethod(wrongMethodName, rightMethod.getParameterTypes());
                if (Build.VERSION.SDK_INT<=19){
                    replaceMethod(Build.VERSION.SDK_INT,wrongMethod,rightMethod);
                }else if (19<Build.VERSION.SDK_INT&&Build.VERSION.SDK_INT<24){
                    replaceArt(wrongMethod,rightMethod);
                }else {
                    replaceArtN(wrongMethod,rightMethod);
                }
            } catch (ClassNotFoundException e) {
                e.printStackTrace();
            } catch (NoSuchMethodException e) {
                e.printStackTrace();
            }
        }
    }

    private native void replaceMethod(int sdk_version,Method wrongMethod, Method rightMethod);

    private native void replaceArt(Method wrongMethod, Method rightMethod);
    private native void replaceArtN(Method wrongMethod, Method rightMethod);
}
