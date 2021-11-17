/*
 * This file is part of Anbox Platform SDK
 *
 * Copyright 2021 Canonical Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANBOX_SDK_INPUT_PROCESSOR_H_
#define ANBOX_SDK_INPUT_PROCESSOR_H_

#include <stdint.h>
#include <stddef.h>
#include <linux/input.h>

/**
 * @brief AnboxInputDeviceType describes the type of device an input event
 * belongs to. Possible device types are: pointer, keyboard, touchpanel.
 */
typedef enum {
  /** A pointer device */
  POINTER = 0,
  /** A keyboard device */
  KEYBOARD,
  /** A touchpanel device */
  TOUCHPANEL,
  /** A gamepad device */
  GAMEPAD,
} AnboxInputDeviceType;

/**
 * @brief AnboxInputEvent is similar to the input_event that is defined in the
 * Linux kernel API. The three member variables type, code and value have the
 * same meaning as defined in the Linux kernel API.
 * Please see https://www.kernel.org/doc/Documentation/input/input.txt for further information.
 */
struct AnboxInputEvent {
  /** Type of the device the input event belongs to. */
  AnboxInputDeviceType device_type;
  /** Id identifying a device of a given type */
  int32_t device_id;
  /** The type of the input event. E.g. EV_KEY stands for a keyboard event.
   * See complete list in /usr/include/linux/input-event-codes.h */
  uint16_t type;
  /** The code of the input event. E.g. KEY_ENTER when you press the ENTER key. */
  uint16_t code;
  /** The value of the input event. E.g. 0 when a keyboard key is released,
   * 1 stands for keyboard key is pressed. */
  int32_t value;
};

namespace anbox {
/**
 * @brief InputProcessor allows a plugin to propagate input events to Anbox which
 * will forward them to the Android container and influence behavior of input device
 * in Android.
 */
class InputProcessor {
  public:
    InputProcessor() = default;
    virtual ~InputProcessor() = default;
    InputProcessor(const InputProcessor &) = delete;
    InputProcessor& operator=(const InputProcessor &) = delete;

    /**
     * @brief Read next available input event.
     *
     * Anbox will call read_event() to query the plugin for the next available
     * input event which is then forwarded to the Android container. If no input
     * event is available and \a timeout is set to -1, the function must block until
     * the next event is available.
     *
     * @param event Pointer to the available event to be sent to the anbox container.
     * @param timeout maximum number of milliseconds to wait for the next available event.
     * The following possible values for \a timeout must be handled:
     * | \a timeout  | Behavior |
     * | ------------- | ------------- |
     * | 0   | Non-blocking mode; return -EIO immediately if no event to process. |
     * | < 0 | Block indefinitely until an event is available. |
     * | > 0 | Wait up to a maximum of \a timeout milliseconds for an event. |
     * @return 0 on success otherwise returns EINVAL on error occurs.
     */
    virtual int read_event(AnboxInputEvent* event, int timeout) = 0;

    /**
     * @brief Inject an input event into AnboxPlatform.
     *
     * This function allows injecting an InputEvent into the platform and letting
     * the InputProcessor handle the input events.
     *
     * @param event an event to be pushed into the internal queue.
     * @return 0 on success, otherwise returns EINVAL on error occurs.
     * @note This function is only used in our test suite to facilitate our automation
     *       tests and it is subject to change at any time.
     **/
    virtual int inject_event(AnboxInputEvent event) = 0;
};
} // namespace anbox

#endif
