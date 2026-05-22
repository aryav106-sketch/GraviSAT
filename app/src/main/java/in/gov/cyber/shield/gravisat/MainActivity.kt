package com.gravisat.shield

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.TextView

class MainActivity : AppCompatActivity() {

    external fun stringFromJNI(): String

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val tv = TextView(this)
        tv.text = stringFromJNI()

        setContentView(tv)
    }

    companion object {
        init {
            System.loadLibrary("gravisat")
        }
    }
}
