import cv2
import mediapipe as mp
import threading
import requests

# Inisialisasi mediapipe
mp_drawing = mp.solutions.drawing_utils
mp_hands = mp.solutions.hands

# Inisialisasi detektor tangan
hands = mp_hands.Hands(static_image_mode=False,
                       max_num_hands=1,
                       min_detection_confidence=0.5,
                       min_tracking_confidence=0.5)

# Konstanta warna
COLOR_RED = (0, 0, 255)
COLOR_GREEN = (0, 255, 0)
COLOR_BLUE = (255, 0, 0)
COLOR_WHITE = (255, 255, 255)

# Inisialisasi tampilan jendela
window_name = "Virtual Buttons"
cv2.namedWindow(window_name, cv2.WINDOW_NORMAL)
cv2.setWindowProperty(window_name, cv2.WND_PROP_FULLSCREEN, cv2.WINDOW_FULLSCREEN)

# Koordinat dan ukuran tombol virtual
button1_pos = (50, 50)
button2_pos = (475, 50)
button_width = 100
button_height = 100

# URL untuk setiap tombol
url_button1 = "http://192.168.4.1/button2"
url_button2 = "http://192.168.4.1/button1"

# Fungsi untuk mengirim permintaan HTTP POST secara asynchronous
def send_post_request_async(url):
    try:
        response = requests.post(url)
        print("Permintaan POST berhasil dikirim ke:", url)
    except requests.exceptions.RequestException as e:
        print("Terjadi kesalahan dalam mengirim permintaan POST:", e)

# Fungsi untuk mengirim permintaan HTTP POST tanpa menunggu respons
def send_post_request_nonblocking(url):
    thread = threading.Thread(target=send_post_request_async, args=(url,))
    thread.start()

# Fungsi untuk menggambar tombol virtual
def draw_button(frame, pos, color, pressed):
    # Menggambar tombol dengan warna dan stroke yang sesuai
    if pressed:
        cv2.rectangle(frame, pos, (pos[0] + button_width, pos[1] + button_height), color, -1)
    else:
        cv2.rectangle(frame, pos, (pos[0] + button_width, pos[1] + button_height), color, 2)

# Fungsi untuk menggambar garis dan titik pada jari-jari
def draw_finger_landmarks(frame, landmarks):
    # Menggambar garis antara titik-titik jari
    mp_drawing.draw_landmarks(frame, landmarks, mp_hands.HAND_CONNECTIONS)

    # Menggambar titik pada setiap jari dengan warna yang sama
    for idx, landmark in enumerate(landmarks.landmark):
        x = int(landmark.x * frame.shape[1])
        y = int(landmark.y * frame.shape[0])
        cv2.circle(frame, (x, y), 5, (0, 255, 0), -1)  # Menggunakan warna hijau (0, 255, 0)


# Inisialisasi status tombol
button1_pressed = False
button2_pressed = False

# Main loop
cap = cv2.VideoCapture(0)
while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        continue

    # Flip frame secara horizontal
    frame = cv2.flip(frame, 1)

    # Konversi frame menjadi BGR ke RGB
    frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

    # Deteksi tangan dan jari-jari
    results = hands.process(frame_rgb)
    if results.multi_hand_landmarks:
        for hand_landmarks in results.multi_hand_landmarks:
            # Menggambar garis dan titik pada jari-jari
            draw_finger_landmarks(frame, hand_landmarks)

            # Mendeteksi koordinat jari telunjuk
            finger_landmark = hand_landmarks.landmark[mp_hands.HandLandmark.INDEX_FINGER_TIP]
            finger_x = int(finger_landmark.x * frame.shape[1])
            finger_y = int(finger_landmark.y * frame.shape[0])

            # Deteksi tombol virtual yang ditekan oleh jari telunjuk
            if finger_x >= button1_pos[0] and finger_x <= button1_pos[0] + button_width and \
                    finger_y >= button1_pos[1] and finger_y <= button1_pos[1] + button_height:
                if not button1_pressed:
                    send_post_request_nonblocking(url_button1)
                button1_pressed = True
            else:
                button1_pressed = False

            if finger_x >= button2_pos[0] and finger_x <= button2_pos[0] + button_width and \
                    finger_y >= button2_pos[1] and finger_y <= button2_pos[1] + button_height:
                if not button2_pressed:
                    send_post_request_nonblocking(url_button2)
                button2_pressed = True
            else:
                button2_pressed = False

    # Menggambar tombol virtual dengan status pressed
    draw_button(frame, button1_pos, COLOR_RED, button1_pressed)
    draw_button(frame, button2_pos, COLOR_BLUE, button2_pressed)

    # Tampilkan frame dengan tombol virtual
    cv2.imshow(window_name, frame)

    # Tombol 'q' untuk keluar dari program
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Stop video capture dan tutup jendela
cap.release()
cv2.destroyAllWindows()
