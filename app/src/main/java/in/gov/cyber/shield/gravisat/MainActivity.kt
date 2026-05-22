package com.gravisat.shield

import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {

    external fun solveSAT(input: String): String

    companion object {

        init {
            System.loadLibrary("gravisat")
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        setContentView(R.layout.activity_main)

        val inputCNF = findViewById<EditText>(R.id.inputCNF)
        val solveButton = findViewById<Button>(R.id.solveButton)
        val resultText = findViewById<TextView>(R.id.resultText)

        solveButton.setOnClickListener {

            val cnf = inputCNF.text.toString()

            val result = solveSAT(cnf)

            resultText.text = result
        }
    }
}
