package android.kotlin.aplikasiiotfirebase

import android.content.ContentValues
import android.kotlin.aplikasiiotfirebase.databinding.ActivityMainBinding
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import com.google.firebase.database.DataSnapshot
import com.google.firebase.database.DatabaseError
import com.google.firebase.database.DatabaseReference
import com.google.firebase.database.ValueEventListener
import com.google.firebase.database.ktx.database
import com.google.firebase.database.ktx.getValue
import com.google.firebase.ktx.Firebase

class MainActivity : AppCompatActivity() {
    private lateinit var database: DatabaseReference
    private lateinit var binding: ActivityMainBinding
    private var power: String = "OFF"
    private lateinit var powerFirebase: String
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        val view = binding.root
        setContentView(view)

        // Write a message to the database
        database = Firebase.database.reference

        val databaseListener = object : ValueEventListener {
            override fun onDataChange(dataSnapshot: DataSnapshot) {
                // Get data dari firebase dan update value ke tampilan UI
                val suhu = dataSnapshot.child("FirebaseIoT/Temperature").getValue<Double>().toString()
                val humidity = dataSnapshot.child("FirebaseIoT/Humidity").getValue<Double>().toString()
                val ldr = dataSnapshot.child("FirebaseIoT/LDR").getValue<Double>().toString()
                binding.vwSuhu.text = "Suhu: $suhu"
                binding.vwHumidity.text = "Humidity: $humidity"
                binding.vwLdr.text = "LDR: $ldr"
                binding.btnPower.setOnClickListener {
                    if (power == "OFF"){
                        power = "ON"
                        binding.btnPower.text = power
                        database.child("FirebaseIoT/Power").setValue(power)
                    } else {
                        power = "OFF"
                        binding.btnPower.text = power
                        database.child("FirebaseIoT/Power").setValue(power)
                    }
                }
            }

            override fun onCancelled(databaseError: DatabaseError) {
                // Getting Post failed, log a message
                Log.w(ContentValues.TAG, "loadPost:onCancelled", databaseError.toException())
            }
        }
        database.addValueEventListener(databaseListener)
    }
}