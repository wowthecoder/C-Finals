#include "layer.h"

// #define ALLOCMEM(ptr, n, typ) = { ptr = calloc(n, sizeof(typ)); return ptr == NULL; }

double sigmoid(double x) {
  return (1.0 / (1.0 + exp(-x)));
}

double sigmoidprime(double x) {
  return x * (1 - x);
}

layer_t *layer_create() {
  return (layer_t *) calloc(1, sizeof(layer_t));
}

bool layer_init(layer_t *layer, int num_outputs, layer_t *prev) {
  layer->prev = prev;
  layer->num_outputs = num_outputs;
  layer->outputs = calloc(num_outputs, sizeof(double));
  if (layer->outputs == NULL) {
    return true;
  }
  if (prev != NULL) {
    layer->num_inputs = prev->num_outputs;
    int num_inputs = layer->num_inputs;
    // Allocate row pointers, each input is 1 row
    layer->weights = malloc(num_inputs * sizeof(double *));
    if (layer->weights == NULL) {
      return true;
    }
    layer->weights[0] = malloc(num_inputs * num_outputs * sizeof(double));
    if (layer->weights[0] == NULL) {
      free ( layer->weights );
      return true;
    }
    for (int i = 0; i < num_inputs; i++) {
      if (i > 0) {
        layer->weights[i] = layer->weights[i-1] + num_outputs;
      }
      // Assign random values
      for (int j = 0; j < num_outputs; j++) {
        layer->weights[i][j] = ANN_RANDOM();
      }
    }
    layer->biases = calloc(num_outputs, sizeof(double));
    layer->deltas = calloc(num_outputs, sizeof(double));
    if (layer->biases == NULL || layer->deltas == NULL) {
      return true;
    }
  } 
  return false;
}

void layer_free(layer_t *layer) {
  free ( layer->outputs );
  if (layer->prev != NULL) {
    free ( layer->weights[0] );
    free ( layer->weights );
    free ( layer->biases );
    free ( layer->deltas );
  }
  free ( layer );
}

void layer_compute_outputs(layer_t const *layer) {
  for (int j = 0; j < layer->num_outputs; j++) {
    double sum_inputs = 0;
    for (int i = 0; i < layer->num_inputs; i++) {
      sum_inputs += (layer->weights[i][j] * layer->prev->outputs[i]);
    }
    layer->outputs[j] = sigmoid(layer->biases[j] + sum_inputs);
  }
}

void layer_compute_deltas(layer_t const *layer) {
    for (int i = 0; i < layer->num_outputs; i++) {
        layer_t *next = layer->next;
        double sum_gradients = 0;
        for (int j = 0; j < next->num_outputs; j++) {
            sum_gradients += next->weights[i][j] * next->deltas[j];
        }
        layer->deltas[i] = sigmoidprime(layer->outputs[i]) * sum_gradients;
    }
}

void layer_update(layer_t const *layer, double l_rate) {
    for (int j = 0; j < layer->num_outputs; j++) {
        for (int i = 0; i < layer->num_inputs; i++) {
            layer->weights[i][j] += l_rate * layer->prev->outputs[i] * layer->deltas[j];
        }
        layer->biases[j] += l_rate * layer->deltas[j];
    }
}