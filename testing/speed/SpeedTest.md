# Speed Test

<!-- Harry Boyd - 03/03/2025 - github.com/hboyd255 -->

The maximum power the robot can handle is 80. If the motors are set to 100%, if
robot stalls, the motors will draw enough current to cause the battery to shut
down.

More testing is needed to determine the current draw of each motor, but for now,
80% is considered the maximum safe power. This is the equivalent to
analogWrite(SPEED_PIN, 204).

## Speed tests

After being allowed to accelerate, the robot was timed traveling 2m forwards.

Times 1.18 1.13 1.21

average: 1.17

which is 1.71 m/s

## brakeing Distance

After being allowed to accelerate, the robot drove to a point then stopped. When
power to the motors was cut, the robot continued to coast for 2.09m. When the
robot was commanded to stop, it stopped after 0.64m.

To brake the robot, the motors were set to full power, and the code 0b11 is sent
to the motor driver.



# Walking Speed

Distance 8.4m
Time 5.2s

Equal to a walk speed of 1.6m/s
