#ifndef ELOQUENTTINYML_ZOO_ANIMAL_DETECTION_H
#define ELOQUENTTINYML_ZOO_ANIMAL_DETECTION_H

#ifndef ANIML_DETECTION_ARENA_SIZE
#define ANIMAL_DETECTION_ARENA_SIZE 90000L
#endif

#include "../tf.h"
#include "../exception.h"
#include "../benchmark.h"
#include "./animal_detection_model.h"

using Eloquent::TF::Sequential;
using Eloquent::Error::Exception;


namespace Eloquent {
    namespace TinyML {
        namespace Zoo {
            /**
             * Run animal detection on 96x96 grayscale image
             */
            class AnimalDetection {
            public:
                Sequential<5, ANIMAL_DETECTION_ARENA_SIZE> tf;
                Exception exception;

                /**
                 * Constructor
                 */
                AnimalDetection() :
                    exception("AnimalDetection"),
                    _thresh(180) {

                }

                /**
                 * Test if a animal is detected
                 */
                operator bool() {
                    if (exception || tf.exception)
                        return false;

                    const uint8_t pos = animalScore();
                    const uint8_t neg = notAnimalScore();

                    return pos > neg && pos >= _thresh;
                }

                /**
                 * Init model
                 * @return
                 */
                Exception& begin() {
                    tf.setNumInputs(96 * 96);
                    tf.setNumOutputs(3);
                    tf.resolver.AddDepthwiseConv2D();
                    tf.resolver.AddConv2D();
                    tf.resolver.AddAveragePool2D();
                    tf.resolver.AddReshape();
                    tf.resolver.AddSoftmax();

                    if (!tf.begin(eloq::tinyml::zoo::animalDetectionModel).isOk())
                        return tf.exception;

                    return exception.clear();
                }

                /**
                 * Run detection on frame
                 * @param image
                 * @return
                 */
                Exception& run(uint8_t *image) {
                    for (int i = 0; i < 96 * 96; i++)
                        _buffer[i] = ((int16_t) image[i]) - 128;

                    if (!tf.predict(_buffer).isOk())
                        return tf.exception;

                    return exception.clear();
                }

                /**
                 * Run detection on camera
                 * @param camera
                 * @return
                 */
                 template<typename Camera>
                Exception& run(Camera& camera) {
                    return run(camera.frame->buf);
                }

                /**
                 *
                 * @return
                 */
                uint8_t animalScore() {
                    return 128 + tf.output(1);
                }

                /**
                 *
                 * @return
                 */
                uint8_t notAnimalScore() {
                    return 128 + tf.output(2);
                }

            protected:
                uint8_t _thresh;
                int8_t _buffer[96 * 96];
            };
        }
    }
}

namespace eloq {
    namespace tinyml {
        namespace zoo {
            static Eloquent::TinyML::Zoo::AnimalDetection animalDetection;
        }
    }
}

#endif //ELOQUENTTINYML_ZOO_ANIMAL_DETECTION_H
