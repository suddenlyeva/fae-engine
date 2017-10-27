# fae-engine
Creating a general purpose version of the Danmakufu scripting engine, coined Fae.

Detailed documentation will go here later.

For now this simple prime sifting script can serve as a decent look into how the language functions.

#prime.fae

// Designates a console loop as the main loop for this program.
// It will advance forwards one step when a newline character is entered.
@Console {
    yield; // This yield restores all tasks and runs them in sequence.
}

// Setup is run once before the loop begins.
// It is used here to start the main task.
@Setup {
    PrimeInfo;
}

// This is the main schedule for this program.
task PrimeInfo {

    // This is an object created using JSON-style syntax.
    let currentNumber = {
        value :   1,
        factors : 1
    };
    
    // This block starts running a series of tasks on the object.
    // Fae guarantees the order of execution of these microthreads.
    currentNumber.Label;
    currentNumber.Factorize
    currentNumber.CheckPrime;
    currentNumber.Update;

    //
    // Local Task Definitions
    //
    
    // First print the label on the side.
    task Label {
        loop {
            print(this.value ~ ": ");
            yield;
        }
    }

    // Then print all the number's factors.
    task Factorize {

        // This will start a new task every step using the new value of the number.
        loop {
            this.Factor(this.value);
            yield;
        }
        
        // All it needs to do is print the number every n steps and count a factor if it does.
        task Factor(n) {
            if (n == 1) {
                loop {
                    print("1");
                    yield;
                }
            }
            else {
                loop {
                    print("," ~ n);
                    this.factors++;
                    wait(n);
                }
            }
        }
    }

    // If the number has 2 factors it is prime.
    task CheckPrime {
        loop {
            if (this.value != 1 && this.factors <= 2) {
                print(" PRIME!");
            }
            yield;
        }
    }

    // Reset the number of factors every step and increment the number.
    task Update {
        loop {
            this.value++;
            this.factors = 1;
            yield;
        }
    }
}

// Common function in task-oriented scripting will block a calling task for n steps.
function wait(n) {
    loop(n) { yield; }
}