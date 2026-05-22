package com.gravisat.shield

import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {

    external fun solveSAT(input: String): String

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        setContentView(R.layout.activity_main)

        val cnfInput = findViewById<EditText>(R.id.cnfInput)
        val solveButton = findViewById<Button>(R.id.solveButton)
        val resultText = findViewById<TextView>(R.id.resultText)

        solveButton.setOnClickListener {

            val cnf = cnfInput.text.toString()

            val result = solveSAT(cnf)

            resultText.text = result
        }
    }

    companion object {

        init {
            System.loadLibrary("gravisat")
        }
    }
}
