package com.example.gravisat

import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.TextView

import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {

    companion object {

        init {

            System.loadLibrary("gravisat")
        }
    }

    external fun solveSAT(
        inputText: String
    ): String

    override fun onCreate(
        savedInstanceState: Bundle?
    ) {

        super.onCreate(
            savedInstanceState)

        setContentView(
            R.layout.activity_main)

        val inputCNF =
            findViewById<EditText>(
                R.id.inputCNF)

        val solveButton =
            findViewById<Button>(
                R.id.solveButton)

        val resultText =
            findViewById<TextView>(
                R.id.resultText)

        inputCNF.setText(

            "p cnf 2 2\n" +
            "1 2 0\n" +
            "-1 2 0"
        )

        solveButton.setOnClickListener {

            resultText.text =
                "Solving..."

            Thread {

                try {

                    val result =
                        solveSAT(
                            inputCNF.text
                                .toString()
                        )

                    runOnUiThread {

                        resultText.text =
                            result
                    }

                } catch (e: Exception) {

                    runOnUiThread {

                        resultText.text =
                            "ERROR:\n${e.message}"
                    }
                }

            }.start()
        }
    }
}
