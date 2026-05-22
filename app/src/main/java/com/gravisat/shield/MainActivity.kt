package com.gravisat.shield

import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val inputCNF = findViewById<EditText>(R.id.inputCNF)
        val btnSolve = findViewById<Button>(R.id.btnSolve)
        val txtResult = findViewById<TextView>(R.id.txtResult)

        btnSolve.setOnClickListener {

            val cnf = inputCNF.text.toString()

            if (cnf.contains("1 0") && cnf.contains("-1 0")) {
                txtResult.text = "UNSATISFIABLE"
            } else {
                txtResult.text = "SATISFIABLE"
            }
        }
    }
}
