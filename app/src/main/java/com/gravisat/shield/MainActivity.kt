package com.gravisat.shield

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.TextView

class MainActivity : AppCompatActivity() {

    external fun solveCNF(input: String): String

    companion object {
        init {
            System.loadLibrary("gravisat")
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val cnfInput = findViewById<EditText>(R.id.cnfInput)
        val solveButton = findViewById<Button>(R.id.solveButton)
        val resultView = findViewById<TextView>(R.id.resultView)

        btnSolve.setOnClickListener {

    val input = cnfInput.text.toString()

    val result = solveCNF(input)

    txtResult.text = result
}
    }
}
