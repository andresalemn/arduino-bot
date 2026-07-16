# Flask Backend & Ngrok Setup

This document covers the steps required to install Python dependencies, configure the environment, run the Flask server locally, and expose it to the internet using `ngrok`.

---

## 1. Prerequisites and Installation

To run the Alexa skill backend, you need to install the Alexa Skills Kit (ASK) SDK and the Flask adapter on your system. Run the following command:

```bash
pip3 install ask-sdk-core flask-ask-sdk Flask
```

---

## 1b. Flask Host Binding

By default, `app.run()` binds to `host='127.0.0.1'` (localhost) and `port=5000`. This is sufficient as long as `ngrok` is explicitly told to forward to `http://localhost:5000`.

If `ngrok` has trouble resolving `localhost` on your machine/network setup, binding Flask to `host='0.0.0.0'` instead makes the app reachable on any local interface, which can resolve the issue.


---

## 2. Exposing Your Server with Ngrok

Because the Alexa service runs in the cloud, it cannot communicate directly with `localhost` or applications behind a local firewall. `ngrok` creates a secure public HTTPS tunnel to a port on your local machine.

---

## 3. Configuring the Endpoint in the Alexa Developer Console

Once `ngrok` is running and providing a public `https` URL, the Alexa Skill needs to be pointed at it:

1. Log in to the [Alexa Developer Console](https://developer.amazon.com/alexa/console/ask).
2. Navigate to the skill.
3. Go to the **Endpoint** section.
4. Select **HTTPS** as the endpoint type.
5. Paste the `https` forwarding URL from `ngrok` into the "Default Region" field.
6. For the SSL certificate setting, choose **"My development endpoint is a sub-domain of a wildcard certificate from a trusted certificate authority"** (this is correct for `ngrok`-issued URLs).
7. Save the endpoint configuration.

> **Note:** The `ngrok` free tier generates a new random URL every time it's restarted. If you stop and restart `ngrok`, you'll need to repeat this step with the new URL before testing the skill again.

---

### Installation
If you do not have `ngrok` installed, download and install it by following these steps:

1. Sign up for a free account at [ngrok.com](https://ngrok.com).
2. Download the `ngrok` package for Linux.
3. Configure your authtoken (provided on your ngrok dashboard):
   ```bash
   ngrok config add-authtoken <YOUR_AUTHTOKEN>
   ```

### Execution Steps

1. **Start the ROS 2 Action Server**:
   Ensure your ROS 2 environment is sourced and launch the remote interface:
   ```bash
   ros2 launch arduinobot_remote remote_interface.launch.py
   ```
   *Note: This starts the C++ task server and the Python Flask backend.*

2. **Expose Port 5000**:
   Open a separate terminal window and expose port `5000` (the default port Flask listens to):
   ```bash
   ngrok http 5000
   ```

3. **Retrieve the HTTPS URL**:
   Identify the `Forwarding` URL in the ngrok terminal output. It will look like:
   ```text
   https://xxxx-xxxx-xx.ngrok-free.dev
   ```
   *Copy this HTTPS URL; you will need it to configure the Alexa Developer Console.*
