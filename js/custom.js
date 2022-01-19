(function ($) {

  Drupal.behaviors.tseq_import_db = {
    attach: function(context, settings) {
      $('.form-item-userValidate').hide();
      if($('#content #console').find('div.error').length !== 0) {
        $('.form-item-userValidate').show();
      }
    }
  };
}(jQuery));
