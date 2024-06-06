#include "ann.h"

ann_t *ann_create(int num_layers, int *layer_outputs)
{
    ann_t *new_ann = malloc(sizeof(ann_t));
    if (new_ann == NULL) {
        return NULL;
    }
    for (int i = 0; i < num_layers; i++) {
        layer_t *curr_layer = layer_create();
        if (curr_layer == NULL) {
            return NULL;
        }
        if (i == 0) {
            new_ann->input_layer = curr_layer;
            new_ann->output_layer = curr_layer;
        }
        else {
            layer_t *last = new_ann->output_layer; // find the original last of the list
            curr_layer->prev = last; // backlink current layer to last
            last->next = curr_layer; // link last to current layer
            new_ann->output_layer = curr_layer; // current layer is now the last of the list
        }
        layer_init(curr_layer, layer_outputs[i], curr_layer->prev);
    }
    return new_ann;
}

/* Frees the space allocated to ann. */
void ann_free(ann_t *ann)
{
    for (layer_t *curr = ann->input_layer; curr != NULL; curr = curr->next) {
        layer_free( curr );
    }
    free( ann );
}

/* Forward run of given ann with inputs. */
void ann_predict(ann_t const *ann, double const *inputs)
{
    // Cannot just assign this pointer for some reason
    // ann->input_layer->outputs = inputs;
    layer_t *in = ann->input_layer;
    for (int i = 0; i < in->num_outputs; i++) {
        in->outputs[i] = inputs[i];
    }
    for (layer_t *curr = in->next; curr != NULL; curr = curr->next) {
        layer_compute_outputs( curr );
    }
}

/* Trains the ann with single backprop update. */
void ann_train(ann_t const *ann, double const *inputs, double const *targets, double l_rate)
{
    /* Sanity checks. */
    assert(ann != NULL);
    assert(inputs != NULL);
    assert(targets != NULL);
    assert(l_rate > 0);

    /* Run forward pass. */
    ann_predict(ann, inputs);

    /* Compute gradients of output layer */
    layer_t *out_layer = ann->output_layer;
    for (int j = 0; j < out_layer->num_outputs; j++) {
        double oj = out_layer->outputs[j];
        out_layer->deltas[j] = sigmoidprime(oj) * (targets[j] - oj);
    }

    /* Backpropagation (exclude input layer) */
    for (layer_t *curr = ann->output_layer->prev; curr != ann->input_layer; curr = curr->prev) {
        layer_compute_deltas(curr);
    }

    /* Gradient descent step */
    for (layer_t *curr = ann->input_layer->next; curr != NULL; curr = curr->next) {
        layer_update(curr, l_rate);
    }
}
