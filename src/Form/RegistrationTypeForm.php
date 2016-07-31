<?php

namespace Drupal\registration\Form;

use Drupal\Core\Entity\EntityForm;
use Drupal\Core\Form\FormStateInterface;

/**
 * Class RegistrationTypeForm.
 *
 * @package Drupal\registration\Form
 */
class RegistrationTypeForm extends EntityForm {

  /**
   * {@inheritdoc}
   */
  public function form(array $form, FormStateInterface $form_state) {
    $form = parent::form($form, $form_state);

    $registration_type = $this->entity;
    $form['label'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Label'),
      '#maxlength' => 255,
      '#default_value' => $registration_type->label(),
      '#description' => $this->t("Label for the Registration type."),
      '#required' => TRUE,
    ];

    $form['id'] = [
      '#type' => 'machine_name',
      '#default_value' => $registration_type->id(),
      '#machine_name' => [
        'exists' => '\Drupal\registration\Entity\RegistrationType::load',
      ],
      '#disabled' => !$registration_type->isNew(),
    ];

    /* You will need additional form elements for your custom properties. */

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    $registration_type = $this->entity;
    $status = $registration_type->save();

    switch ($status) {
      case SAVED_NEW:
        drupal_set_message($this->t('Created the %label Registration type.', [
          '%label' => $registration_type->label(),
        ]));
        break;

      default:
        drupal_set_message($this->t('Saved the %label Registration type.', [
          '%label' => $registration_type->label(),
        ]));
    }
    $form_state->setRedirectUrl($registration_type->urlInfo('collection'));
  }

}
