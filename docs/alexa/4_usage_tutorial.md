# Usage Tutorial: Controlling Arduinobot with Alexa

This document assumes the full setup described in `3_flask_ngrok_setup.md` is complete and the Alexa Skill endpoint is already configured in the Developer Console. It walks through actually using the skill, plus common issues you might hit.

---

## 1. Pre-Flight Checklist

Before talking to Alexa, confirm the following are running:

- [ ] ROS 2 environment sourced, and `remote_interface.launch.py` is running (starts both the task server and the Flask backend).
- [ ] `ngrok http 5000` is running in a separate terminal, and shows an active `Forwarding` URL.
- [ ] The Alexa Developer Console endpoint matches the **current** ngrok URL (see note below — this changes on restart with the free tier).
- [ ] Your Alexa-enabled device (or the Alexa app / Developer Console test simulator) is logged into the same Amazon developer account the skill is registered under.

> If any of these is stale or down, the skill will either fail silently or Alexa will respond with a generic error — see Troubleshooting below.

---

## 2. Trying It Out

Interactions follow the intents defined in `1_introduction.md`. Say the following to your Alexa device (replace "Arduinobot" with your skill's actual invocation name):

| You say | Intent triggered | Expected robot behavior | Expected Alexa response |
|---|---|---|---|
| "Alexa, open my Arduinobot" | `LaunchRequest` | Moves to home/ready position | "Hi Andres, how can I help?" |
| "Alexa, tell my Arduinobot to pick" | `PickIntent` | Arm moves to pick position, gripper closes | "Ok, I'm moving" |
| "Alexa, tell my Arduinobot to sleep" | `SleepIntent` | Arm returns to sleep position | Confirmation response |

A successful run looks like: you hear Alexa's spoken confirmation almost immediately (the "Ok, I'm moving" response), and the robot's physical motion follows shortly after — the trajectory execution happens after the HTTP response is sent, so don't expect them to be simultaneous (see the sequence diagram in `2_architecture.md`).

---

## 3. Troubleshooting

**Alexa says something like "There was a problem with the requested skill's response" or times out:**
- Check the ngrok terminal — is the tunnel still active? Free-tier ngrok sessions can time out or the URL can change on restart. If the URL changed, update it in the Developer Console endpoint (`3_flask_ngrok_setup.md`, section 3) and save.
- Check the Flask terminal for a stack trace — a handler exception will usually show up here immediately after the request comes in.

**Alexa responds with speech, but the robot doesn't move:**
- Confirm the ROS 2 Action Client successfully connected to the `task_server` node. If the task server started after the Flask app, or crashed separately, the action goal may be silently failing to send.
- Check that `remote_interface.launch.py` actually brought up both the C++ task server and the Python Flask backend — if only one came up, restart the launch file.

**Nothing happens at all — Alexa doesn't respond, not even an error:**
- Verify the ngrok forwarding URL in the console matches exactly what's currently in the ngrok terminal (including `https://`, no trailing slash mismatches).
- Confirm Flask is bound to the port ngrok is forwarding to (default `5000`). If you're using `host='0.0.0.0'` per `3_flask_ngrok_setup.md`, double check ngrok is still pointed at the same port.

**Skill works in the Developer Console test simulator but not on the physical device:**
- Confirm the physical device is registered to the same Amazon account as the skill's development registration — skills in development mode are only accessible to devices under that account.

---

## See Also

- [`1_introduction.md`](./1_introduction.md) — Alexa Skill concepts (intents, utterances, launch requests)
- [`2_architecture.md`](./2_architecture.md) — Full request flow and ROS 2 task mapping
- [`3_flask_ngrok_setup.md`](./3_flask_ngrok_setup.md) — Installation and endpoint configuration