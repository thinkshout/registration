<?php

namespace Drupal\registration\Entity;

use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\Core\Entity\EntityChangedInterface;
use Drupal\user\EntityOwnerInterface;

/**
 * Provides an interface for defining Registration type entities.
 *
 * @ingroup registration
 */
interface RegistrationTypeInterface extends ContentEntityInterface, EntityChangedInterface, EntityOwnerInterface {

  // Add get/set methods for your configuration properties here.

  /**
   * Gets the Registration type type.
   *
   * @return string
   *   The Registration type type.
   */
  public function getType();

  /**
   * Gets the Registration type name.
   *
   * @return string
   *   Name of the Registration type.
   */
  public function getName();

  /**
   * Sets the Registration type name.
   *
   * @param string $name
   *   The Registration type name.
   *
   * @return \Drupal\registration\Entity\RegistrationTypeInterface
   *   The called Registration type entity.
   */
  public function setName($name);

  /**
   * Gets the Registration type creation timestamp.
   *
   * @return int
   *   Creation timestamp of the Registration type.
   */
  public function getCreatedTime();

  /**
   * Sets the Registration type creation timestamp.
   *
   * @param int $timestamp
   *   The Registration type creation timestamp.
   *
   * @return \Drupal\registration\Entity\RegistrationTypeInterface
   *   The called Registration type entity.
   */
  public function setCreatedTime($timestamp);

  /**
   * Returns the Registration type published status indicator.
   *
   * Unpublished Registration type are only visible to restricted users.
   *
   * @return bool
   *   TRUE if the Registration type is published.
   */
  public function isPublished();

  /**
   * Sets the published status of a Registration type.
   *
   * @param bool $published
   *   TRUE to set this Registration type to published, FALSE to set it to unpublished.
   *
   * @return \Drupal\registration\Entity\RegistrationTypeInterface
   *   The called Registration type entity.
   */
  public function setPublished($published);

}
