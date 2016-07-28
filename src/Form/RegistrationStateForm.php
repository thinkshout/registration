<?php

namespace Drupal\registration\Form;

use Drupal\Core\Entity\EntityForm;
use Drupal\Core\Form\FormStateInterface;

/**
 * Class RegistrationStateForm.
 *
 * @package Drupal\registration\Form
 */
class RegistrationStateForm extends EntityForm {

  /**
   * {@inheritdoc}
   */
  public function form(array $form, FormStateInterface $form_state) {
    $form = parent::form($form, $form_state);

    $registration_state = $this->entity;
    $form['label'] = array(
      '#type' => 'textfield',
      '#title' => $this->t('Label'),
      '#maxlength' => 255,
      '#default_value' => $registration_state->label(),
      '#description' => $this->t("Label for the Registration state."),
      '#required' => TRUE,
    );

    $form['id'] = array(
      '#type' => 'machine_name',
      '#default_value' => $registration_state->id(),
      '#machine_name' => array(
        'exists' => '\Drupal\registration\Entity\RegistrationState::load',
      ),
      '#disabled' => !$registration_state->isNew(),
    );

    /* You will need additional form elements for your custom properties. */

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    $registration_state = $this->entity;
    $status = $registration_state->save();

    switch ($status) {
      case SAVED_NEW:
        drupal_set_message($this->t('Created the %label Registration state.', [
          '%label' => $registration_state->label(),
        ]));
        break;

      default:
        drupal_set_message($this->t('Saved the %label Registration state.', [
          '%label' => $registration_state->label(),
        ]));
    }
    $form_state->setRedirectUrl($registration_state->urlInfo('collection'));
  }

}
