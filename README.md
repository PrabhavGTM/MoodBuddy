# MoodBuddy
🌟 Mood Buddy

Mood Buddy is an interactive Arduino-powered desk companion designed to help college students stay positive, mindful, and balanced. It senses your mood using buttons, sound, and motion, then responds with expressive faces, calming sounds, and motivational messages — a mix of engineering and empathy right on your desk.

🧠 Inspiration

College life can get stressful, and it’s easy to forget to take care of yourself. Mood Buddy was inspired by the idea of building a supportive companion that reacts to your environment and reminds you to breathe, stretch, and smile — bringing a bit of emotional intelligence into hardware form.

⚙️ What It Does

Detects motion using a PIR sensor to greet you when you sit down.

Monitors noise levels with a sound sensor and reminds you to calm down when things get too loud.

Responds to button inputs (happy, sad, or calm) with matching expressions, melodies, and uplifting messages.

Displays animated faces on the OLED screen and text messages on the LCD screen.

Encourages healthy habits with gentle reminders every 30 minutes to stretch or take a break.

💻 How the Code Works

The project is built around an Arduino Mega, controlling multiple components simultaneously:

The main loop constantly checks motion, sound, and button states using non-blocking timing (millis() instead of delay()) so all systems stay responsive.

When motion is detected, the bot plays a greeting melody and displays a friendly message.

If sound exceeds a set threshold, the tooLoudResponse() function activates — showing a calm face and gentle reminder to relax.

Button presses trigger one of three mood reactions:

reactHappy() – plays an upbeat tune, smiles, and shows motivational messages.

reactSad() – displays comforting text and soothing tones.

reactCalm() – promotes mindfulness through slow melodies and breathing reminders.

Every 30 minutes (tracked with millis()), showStretchReminder() displays a stretch reminder and plays a short chime.

OLED animations are drawn using the U8g2 library, while the LiquidCrystal library displays supportive text messages.

The program is structured with modular functions for each reaction and facial expression, making it easy to update messages, change melodies, or add new moods later.

🧩 Built With

Arduino Mega

C/C++

U8g2 Library – for OLED face animations

LiquidCrystal Library – for displaying text

PIR Motion Sensor

Sound Sensor

Push Buttons

Piezo Buzzer

OLED & LCD Displays
