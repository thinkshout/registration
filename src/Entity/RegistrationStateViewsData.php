<?php

namespace Drupal\registration\Entity;

use Drupal\views\EntityViewsData;
use Drupal\views\EntityViewsDataInterface;

/**
 * Provides Views data for Registration state entities.
 */
class RegistrationStateViewsData extends EntityViewsData implements EntityViewsDataInterface {

  /**
   * {@inheritdoc}
   */
  public function getViewsData() {
    $data = parent::getViewsData();

    $data['registration_state']['table']['base'] = array(
      'field' => 'id',
      'title' => $this->t('Registration state'),
      'help' => $this->t('The Registration state ID.'),
    );

    return $data;
  }

}
