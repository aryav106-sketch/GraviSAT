package com.example.gravisat

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent

import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.*

import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp

import kotlinx.coroutines.*
import androidx.compose.ui.text.font.FontFamily

class MainActivity : ComponentActivity() {

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
            savedInstanceState
        )

        setContent {

            MaterialTheme {

                SolverScreen(
                    solveNative = ::solveSAT
                )
            }
        }
    }
}

@Composable
fun SolverScreen(
    solveNative: (String) -> String
) {

    var cnfText by remember {

        mutableStateOf(
            "p cnf 2 2\n1 2 0\n-1 2 0"
        )
    }

    var resultText by remember {

        mutableStateOf("")
    }

    var solving by remember {

        mutableStateOf(false)
    }

    val scope =
        rememberCoroutineScope()

    Column(

        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp)

    ) {

        Text(

            text = "GraviSAT PRIME",

            style =
                MaterialTheme
                    .typography
                    .headlineMedium
        )

        Spacer(
            modifier =
                Modifier.height(16.dp)
        )

        OutlinedTextField(

            value = cnfText,

            onValueChange = {

                cnfText = it
            },

            modifier = Modifier
                .fillMaxWidth()
                .height(240.dp),

            label = {

                Text("DIMACS CNF")
            },

            textStyle =
                LocalTextStyle.current.copy(
                    fontFamily =
                        FontFamily.Monospace
                )
        )

        Spacer(
            modifier =
                Modifier.height(16.dp)
        )

        Button(

            onClick = {

                solving = true

                resultText =
                    "Solving..."

                scope.launch(

                    Dispatchers.Default

                ) {

                    val result =
                        solveNative(
                            cnfText
                        )

                    withContext(
                        Dispatchers.Main
                    ) {

                        resultText =
                            result

                        solving =
                            false
                    }
                }
            },

            enabled = !solving

        ) {

            Text(

                if (solving)
                    "Running..."
                else
                    "Solve"
            )
        }

        Spacer(
            modifier =
                Modifier.height(16.dp)
        )

        Text(

            text = resultText,

            style =
                MaterialTheme
                    .typography
                    .bodyMedium,

            fontFamily =
                FontFamily.Monospace
        )
    }
}
