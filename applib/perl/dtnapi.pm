# This file was automatically generated by SWIG (http://www.swig.org).
# Version 1.3.35
#
# Don't modify this file, modify the SWIG interface instead.

package dtnapi;
require Exporter;
require DynaLoader;
@ISA = qw(Exporter DynaLoader);
package dtnapic;
bootstrap dtnapi;
package dtnapi;
@EXPORT = qw( );

# ---------- BASE METHODS -------------

package dtnapi;

sub TIEHASH {
    my ($classname,$obj) = @_;
    return bless $obj, $classname;
}

sub CLEAR { }

sub FIRSTKEY { }

sub NEXTKEY { }

sub FETCH {
    my ($self,$field) = @_;
    my $member_func = "swig_${field}_get";
    $self->$member_func();
}

sub STORE {
    my ($self,$field,$newval) = @_;
    my $member_func = "swig_${field}_set";
    $self->$member_func($newval);
}

sub this {
    my $ptr = shift;
    return tied(%$ptr);
}


# ------- FUNCTION WRAPPERS --------

package dtnapi;

*xdr_dtn_endpoint_id_t = *dtnapic::xdr_dtn_endpoint_id_t;
*xdr_dtn_reg_id_t = *dtnapic::xdr_dtn_reg_id_t;
*xdr_dtn_timeval_t = *dtnapic::xdr_dtn_timeval_t;
*xdr_dtn_timestamp_t = *dtnapic::xdr_dtn_timestamp_t;
*xdr_dtn_service_tag_t = *dtnapic::xdr_dtn_service_tag_t;
*xdr_dtn_reg_flags_t = *dtnapic::xdr_dtn_reg_flags_t;
*xdr_dtn_reg_info_t = *dtnapic::xdr_dtn_reg_info_t;
*xdr_dtn_bundle_priority_t = *dtnapic::xdr_dtn_bundle_priority_t;
*xdr_dtn_bundle_delivery_opts_t = *dtnapic::xdr_dtn_bundle_delivery_opts_t;
*xdr_dtn_extension_block_flags_t = *dtnapic::xdr_dtn_extension_block_flags_t;
*xdr_dtn_extension_block_t = *dtnapic::xdr_dtn_extension_block_t;
*xdr_dtn_sequence_id_t = *dtnapic::xdr_dtn_sequence_id_t;
*xdr_dtn_bundle_spec_t = *dtnapic::xdr_dtn_bundle_spec_t;
*xdr_dtn_bundle_id_t = *dtnapic::xdr_dtn_bundle_id_t;
*xdr_dtn_status_report_reason_t = *dtnapic::xdr_dtn_status_report_reason_t;
*xdr_dtn_status_report_flags_t = *dtnapic::xdr_dtn_status_report_flags_t;
*xdr_dtn_bundle_status_report_t = *dtnapic::xdr_dtn_bundle_status_report_t;
*xdr_dtn_bundle_payload_location_t = *dtnapic::xdr_dtn_bundle_payload_location_t;
*xdr_dtn_bundle_payload_t = *dtnapic::xdr_dtn_bundle_payload_t;
*dtn_strerror = *dtnapic::dtn_strerror;
*dtn_open = *dtnapic::dtn_open;
*find_handle = *dtnapic::find_handle;
*dtn_close = *dtnapic::dtn_close;
*dtn_errno = *dtnapic::dtn_errno;
*dtn_build_local_eid = *dtnapic::dtn_build_local_eid;
*build_reginfo = *dtnapic::build_reginfo;
*dtn_register = *dtnapic::dtn_register;
*dtn_unregister = *dtnapic::dtn_unregister;
*dtn_find_registration = *dtnapic::dtn_find_registration;
*dtn_change_registration = *dtnapic::dtn_change_registration;
*dtn_bind = *dtnapic::dtn_bind;
*dtn_unbind = *dtnapic::dtn_unbind;
*dtn_send = *dtnapic::dtn_send;
*dtn_cancel = *dtnapic::dtn_cancel;
*dtn_status_report_reason_to_str = *dtnapic::dtn_status_report_reason_to_str;
*dtn_recv = *dtnapic::dtn_recv;
*dtn_session_update = *dtnapic::dtn_session_update;
*dtn_poll_fd = *dtnapic::dtn_poll_fd;
*dtn_begin_poll = *dtnapic::dtn_begin_poll;
*dtn_cancel_poll = *dtnapic::dtn_cancel_poll;

############# Class : dtnapi::dtn_endpoint_id_t ##############

package dtnapi::dtn_endpoint_id_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( dtnapi );
%OWNER = ();
%ITERATORS = ();
*swig_uri_get = *dtnapic::dtn_endpoint_id_t_uri_get;
*swig_uri_set = *dtnapic::dtn_endpoint_id_t_uri_set;
sub new {
    my $pkg = shift;
    my $self = dtnapic::new_dtn_endpoint_id_t(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        dtnapic::delete_dtn_endpoint_id_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : dtnapi::dtn_timestamp_t ##############

package dtnapi::dtn_timestamp_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( dtnapi );
%OWNER = ();
%ITERATORS = ();
*swig_secs_get = *dtnapic::dtn_timestamp_t_secs_get;
*swig_secs_set = *dtnapic::dtn_timestamp_t_secs_set;
*swig_seqno_get = *dtnapic::dtn_timestamp_t_seqno_get;
*swig_seqno_set = *dtnapic::dtn_timestamp_t_seqno_set;
sub new {
    my $pkg = shift;
    my $self = dtnapic::new_dtn_timestamp_t(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        dtnapic::delete_dtn_timestamp_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : dtnapi::dtn_service_tag_t ##############

package dtnapi::dtn_service_tag_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( dtnapi );
%OWNER = ();
%ITERATORS = ();
*swig_tag_get = *dtnapic::dtn_service_tag_t_tag_get;
*swig_tag_set = *dtnapic::dtn_service_tag_t_tag_set;
sub new {
    my $pkg = shift;
    my $self = dtnapic::new_dtn_service_tag_t(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        dtnapic::delete_dtn_service_tag_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : dtnapi::dtn_reg_info_t ##############

package dtnapi::dtn_reg_info_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( dtnapi );
%OWNER = ();
%ITERATORS = ();
*swig_endpoint_get = *dtnapic::dtn_reg_info_t_endpoint_get;
*swig_endpoint_set = *dtnapic::dtn_reg_info_t_endpoint_set;
*swig_regid_get = *dtnapic::dtn_reg_info_t_regid_get;
*swig_regid_set = *dtnapic::dtn_reg_info_t_regid_set;
*swig_flags_get = *dtnapic::dtn_reg_info_t_flags_get;
*swig_flags_set = *dtnapic::dtn_reg_info_t_flags_set;
*swig_expiration_get = *dtnapic::dtn_reg_info_t_expiration_get;
*swig_expiration_set = *dtnapic::dtn_reg_info_t_expiration_set;
*swig_init_passive_get = *dtnapic::dtn_reg_info_t_init_passive_get;
*swig_init_passive_set = *dtnapic::dtn_reg_info_t_init_passive_set;
*swig_script_get = *dtnapic::dtn_reg_info_t_script_get;
*swig_script_set = *dtnapic::dtn_reg_info_t_script_set;
sub new {
    my $pkg = shift;
    my $self = dtnapic::new_dtn_reg_info_t(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        dtnapic::delete_dtn_reg_info_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : dtnapi::dtn_reg_info_t_script ##############

package dtnapi::dtn_reg_info_t_script;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( dtnapi );
%OWNER = ();
%ITERATORS = ();
*swig_script_len_get = *dtnapic::dtn_reg_info_t_script_script_len_get;
*swig_script_len_set = *dtnapic::dtn_reg_info_t_script_script_len_set;
*swig_script_val_get = *dtnapic::dtn_reg_info_t_script_script_val_get;
*swig_script_val_set = *dtnapic::dtn_reg_info_t_script_script_val_set;
sub new {
    my $pkg = shift;
    my $self = dtnapic::new_dtn_reg_info_t_script(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        dtnapic::delete_dtn_reg_info_t_script($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : dtnapi::dtn_extension_block_t ##############

package dtnapi::dtn_extension_block_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( dtnapi );
%OWNER = ();
%ITERATORS = ();
*swig_type_get = *dtnapic::dtn_extension_block_t_type_get;
*swig_type_set = *dtnapic::dtn_extension_block_t_type_set;
*swig_flags_get = *dtnapic::dtn_extension_block_t_flags_get;
*swig_flags_set = *dtnapic::dtn_extension_block_t_flags_set;
*swig_data_get = *dtnapic::dtn_extension_block_t_data_get;
*swig_data_set = *dtnapic::dtn_extension_block_t_data_set;
sub new {
    my $pkg = shift;
    my $self = dtnapic::new_dtn_extension_block_t(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        dtnapic::delete_dtn_extension_block_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : dtnapi::dtn_extension_block_t_data ##############

package dtnapi::dtn_extension_block_t_data;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( dtnapi );
%OWNER = ();
%ITERATORS = ();
*swig_data_len_get = *dtnapic::dtn_extension_block_t_data_data_len_get;
*swig_data_len_set = *dtnapic::dtn_extension_block_t_data_data_len_set;
*swig_data_val_get = *dtnapic::dtn_extension_block_t_data_data_val_get;
*swig_data_val_set = *dtnapic::dtn_extension_block_t_data_data_val_set;
sub new {
    my $pkg = shift;
    my $self = dtnapic::new_dtn_extension_block_t_data(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        dtnapic::delete_dtn_extension_block_t_data($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : dtnapi::dtn_sequence_id_t ##############

package dtnapi::dtn_sequence_id_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( dtnapi );
%OWNER = ();
%ITERATORS = ();
*swig_data_get = *dtnapic::dtn_sequence_id_t_data_get;
*swig_data_set = *dtnapic::dtn_sequence_id_t_data_set;
sub new {
    my $pkg = shift;
    my $self = dtnapic::new_dtn_sequence_id_t(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        dtnapic::delete_dtn_sequence_id_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : dtnapi::dtn_sequence_id_t_data ##############

package dtnapi::dtn_sequence_id_t_data;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( dtnapi );
%OWNER = ();
%ITERATORS = ();
*swig_data_len_get = *dtnapic::dtn_sequence_id_t_data_data_len_get;
*swig_data_len_set = *dtnapic::dtn_sequence_id_t_data_data_len_set;
*swig_data_val_get = *dtnapic::dtn_sequence_id_t_data_data_val_get;
*swig_data_val_set = *dtnapic::dtn_sequence_id_t_data_data_val_set;
sub new {
    my $pkg = shift;
    my $self = dtnapic::new_dtn_sequence_id_t_data(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        dtnapic::delete_dtn_sequence_id_t_data($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : dtnapi::dtn_bundle_spec_t ##############

package dtnapi::dtn_bundle_spec_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( dtnapi );
%OWNER = ();
%ITERATORS = ();
*swig_source_get = *dtnapic::dtn_bundle_spec_t_source_get;
*swig_source_set = *dtnapic::dtn_bundle_spec_t_source_set;
*swig_dest_get = *dtnapic::dtn_bundle_spec_t_dest_get;
*swig_dest_set = *dtnapic::dtn_bundle_spec_t_dest_set;
*swig_replyto_get = *dtnapic::dtn_bundle_spec_t_replyto_get;
*swig_replyto_set = *dtnapic::dtn_bundle_spec_t_replyto_set;
*swig_priority_get = *dtnapic::dtn_bundle_spec_t_priority_get;
*swig_priority_set = *dtnapic::dtn_bundle_spec_t_priority_set;
*swig_dopts_get = *dtnapic::dtn_bundle_spec_t_dopts_get;
*swig_dopts_set = *dtnapic::dtn_bundle_spec_t_dopts_set;
*swig_expiration_get = *dtnapic::dtn_bundle_spec_t_expiration_get;
*swig_expiration_set = *dtnapic::dtn_bundle_spec_t_expiration_set;
*swig_creation_ts_get = *dtnapic::dtn_bundle_spec_t_creation_ts_get;
*swig_creation_ts_set = *dtnapic::dtn_bundle_spec_t_creation_ts_set;
*swig_delivery_regid_get = *dtnapic::dtn_bundle_spec_t_delivery_regid_get;
*swig_delivery_regid_set = *dtnapic::dtn_bundle_spec_t_delivery_regid_set;
*swig_sequence_id_get = *dtnapic::dtn_bundle_spec_t_sequence_id_get;
*swig_sequence_id_set = *dtnapic::dtn_bundle_spec_t_sequence_id_set;
*swig_obsoletes_id_get = *dtnapic::dtn_bundle_spec_t_obsoletes_id_get;
*swig_obsoletes_id_set = *dtnapic::dtn_bundle_spec_t_obsoletes_id_set;
*swig_metadata_get = *dtnapic::dtn_bundle_spec_t_metadata_get;
*swig_metadata_set = *dtnapic::dtn_bundle_spec_t_metadata_set;
*swig_blocks_get = *dtnapic::dtn_bundle_spec_t_blocks_get;
*swig_blocks_set = *dtnapic::dtn_bundle_spec_t_blocks_set;
sub new {
    my $pkg = shift;
    my $self = dtnapic::new_dtn_bundle_spec_t(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        dtnapic::delete_dtn_bundle_spec_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : dtnapi::dtn_bundle_spec_t_metadata ##############

package dtnapi::dtn_bundle_spec_t_metadata;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( dtnapi );
%OWNER = ();
%ITERATORS = ();
*swig_metadata_len_get = *dtnapic::dtn_bundle_spec_t_metadata_metadata_len_get;
*swig_metadata_len_set = *dtnapic::dtn_bundle_spec_t_metadata_metadata_len_set;
*swig_metadata_val_get = *dtnapic::dtn_bundle_spec_t_metadata_metadata_val_get;
*swig_metadata_val_set = *dtnapic::dtn_bundle_spec_t_metadata_metadata_val_set;
sub new {
    my $pkg = shift;
    my $self = dtnapic::new_dtn_bundle_spec_t_metadata(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        dtnapic::delete_dtn_bundle_spec_t_metadata($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : dtnapi::dtn_bundle_spec_t_blocks ##############

package dtnapi::dtn_bundle_spec_t_blocks;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( dtnapi );
%OWNER = ();
%ITERATORS = ();
*swig_blocks_len_get = *dtnapic::dtn_bundle_spec_t_blocks_blocks_len_get;
*swig_blocks_len_set = *dtnapic::dtn_bundle_spec_t_blocks_blocks_len_set;
*swig_blocks_val_get = *dtnapic::dtn_bundle_spec_t_blocks_blocks_val_get;
*swig_blocks_val_set = *dtnapic::dtn_bundle_spec_t_blocks_blocks_val_set;
sub new {
    my $pkg = shift;
    my $self = dtnapic::new_dtn_bundle_spec_t_blocks(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        dtnapic::delete_dtn_bundle_spec_t_blocks($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : dtnapi::dtn_bundle_id_t ##############

package dtnapi::dtn_bundle_id_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( dtnapi );
%OWNER = ();
%ITERATORS = ();
*swig_source_get = *dtnapic::dtn_bundle_id_t_source_get;
*swig_source_set = *dtnapic::dtn_bundle_id_t_source_set;
*swig_creation_ts_get = *dtnapic::dtn_bundle_id_t_creation_ts_get;
*swig_creation_ts_set = *dtnapic::dtn_bundle_id_t_creation_ts_set;
*swig_frag_offset_get = *dtnapic::dtn_bundle_id_t_frag_offset_get;
*swig_frag_offset_set = *dtnapic::dtn_bundle_id_t_frag_offset_set;
*swig_orig_length_get = *dtnapic::dtn_bundle_id_t_orig_length_get;
*swig_orig_length_set = *dtnapic::dtn_bundle_id_t_orig_length_set;
sub new {
    my $pkg = shift;
    my $self = dtnapic::new_dtn_bundle_id_t(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        dtnapic::delete_dtn_bundle_id_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : dtnapi::dtn_bundle_status_report_t ##############

package dtnapi::dtn_bundle_status_report_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( dtnapi );
%OWNER = ();
%ITERATORS = ();
*swig_bundle_id_get = *dtnapic::dtn_bundle_status_report_t_bundle_id_get;
*swig_bundle_id_set = *dtnapic::dtn_bundle_status_report_t_bundle_id_set;
*swig_reason_get = *dtnapic::dtn_bundle_status_report_t_reason_get;
*swig_reason_set = *dtnapic::dtn_bundle_status_report_t_reason_set;
*swig_flags_get = *dtnapic::dtn_bundle_status_report_t_flags_get;
*swig_flags_set = *dtnapic::dtn_bundle_status_report_t_flags_set;
*swig_receipt_ts_get = *dtnapic::dtn_bundle_status_report_t_receipt_ts_get;
*swig_receipt_ts_set = *dtnapic::dtn_bundle_status_report_t_receipt_ts_set;
*swig_custody_ts_get = *dtnapic::dtn_bundle_status_report_t_custody_ts_get;
*swig_custody_ts_set = *dtnapic::dtn_bundle_status_report_t_custody_ts_set;
*swig_forwarding_ts_get = *dtnapic::dtn_bundle_status_report_t_forwarding_ts_get;
*swig_forwarding_ts_set = *dtnapic::dtn_bundle_status_report_t_forwarding_ts_set;
*swig_delivery_ts_get = *dtnapic::dtn_bundle_status_report_t_delivery_ts_get;
*swig_delivery_ts_set = *dtnapic::dtn_bundle_status_report_t_delivery_ts_set;
*swig_deletion_ts_get = *dtnapic::dtn_bundle_status_report_t_deletion_ts_get;
*swig_deletion_ts_set = *dtnapic::dtn_bundle_status_report_t_deletion_ts_set;
*swig_ack_by_app_ts_get = *dtnapic::dtn_bundle_status_report_t_ack_by_app_ts_get;
*swig_ack_by_app_ts_set = *dtnapic::dtn_bundle_status_report_t_ack_by_app_ts_set;
sub new {
    my $pkg = shift;
    my $self = dtnapic::new_dtn_bundle_status_report_t(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        dtnapic::delete_dtn_bundle_status_report_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : dtnapi::dtn_bundle_payload_t ##############

package dtnapi::dtn_bundle_payload_t;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( dtnapi );
%OWNER = ();
%ITERATORS = ();
*swig_location_get = *dtnapic::dtn_bundle_payload_t_location_get;
*swig_location_set = *dtnapic::dtn_bundle_payload_t_location_set;
*swig_status_report_get = *dtnapic::dtn_bundle_payload_t_status_report_get;
*swig_status_report_set = *dtnapic::dtn_bundle_payload_t_status_report_set;
*swig_buf_get = *dtnapic::dtn_bundle_payload_t_buf_get;
*swig_buf_set = *dtnapic::dtn_bundle_payload_t_buf_set;
*swig_filename_get = *dtnapic::dtn_bundle_payload_t_filename_get;
*swig_filename_set = *dtnapic::dtn_bundle_payload_t_filename_set;
sub new {
    my $pkg = shift;
    my $self = dtnapic::new_dtn_bundle_payload_t(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        dtnapic::delete_dtn_bundle_payload_t($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : dtnapi::dtn_bundle_payload_t_buf ##############

package dtnapi::dtn_bundle_payload_t_buf;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( dtnapi );
%OWNER = ();
%ITERATORS = ();
*swig_buf_len_get = *dtnapic::dtn_bundle_payload_t_buf_buf_len_get;
*swig_buf_len_set = *dtnapic::dtn_bundle_payload_t_buf_buf_len_set;
*swig_buf_val_get = *dtnapic::dtn_bundle_payload_t_buf_buf_val_get;
*swig_buf_val_set = *dtnapic::dtn_bundle_payload_t_buf_buf_val_set;
sub new {
    my $pkg = shift;
    my $self = dtnapic::new_dtn_bundle_payload_t_buf(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        dtnapic::delete_dtn_bundle_payload_t_buf($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : dtnapi::dtn_bundle_payload_t_filename ##############

package dtnapi::dtn_bundle_payload_t_filename;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( dtnapi );
%OWNER = ();
%ITERATORS = ();
*swig_filename_len_get = *dtnapic::dtn_bundle_payload_t_filename_filename_len_get;
*swig_filename_len_set = *dtnapic::dtn_bundle_payload_t_filename_filename_len_set;
*swig_filename_val_get = *dtnapic::dtn_bundle_payload_t_filename_filename_val_get;
*swig_filename_val_set = *dtnapic::dtn_bundle_payload_t_filename_filename_val_set;
sub new {
    my $pkg = shift;
    my $self = dtnapic::new_dtn_bundle_payload_t_filename(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        dtnapic::delete_dtn_bundle_payload_t_filename($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : dtnapi::dtn_bundle_id ##############

package dtnapi::dtn_bundle_id;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( dtnapi );
%OWNER = ();
%ITERATORS = ();
*swig_source_get = *dtnapic::dtn_bundle_id_source_get;
*swig_source_set = *dtnapic::dtn_bundle_id_source_set;
*swig_creation_secs_get = *dtnapic::dtn_bundle_id_creation_secs_get;
*swig_creation_secs_set = *dtnapic::dtn_bundle_id_creation_secs_set;
*swig_creation_seqno_get = *dtnapic::dtn_bundle_id_creation_seqno_get;
*swig_creation_seqno_set = *dtnapic::dtn_bundle_id_creation_seqno_set;
sub new {
    my $pkg = shift;
    my $self = dtnapic::new_dtn_bundle_id(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        dtnapic::delete_dtn_bundle_id($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : dtnapi::dtn_status_report ##############

package dtnapi::dtn_status_report;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( dtnapi );
%OWNER = ();
%ITERATORS = ();
*swig_bundle_id_get = *dtnapic::dtn_status_report_bundle_id_get;
*swig_bundle_id_set = *dtnapic::dtn_status_report_bundle_id_set;
*swig_reason_get = *dtnapic::dtn_status_report_reason_get;
*swig_reason_set = *dtnapic::dtn_status_report_reason_set;
*swig_flags_get = *dtnapic::dtn_status_report_flags_get;
*swig_flags_set = *dtnapic::dtn_status_report_flags_set;
*swig_receipt_ts_secs_get = *dtnapic::dtn_status_report_receipt_ts_secs_get;
*swig_receipt_ts_secs_set = *dtnapic::dtn_status_report_receipt_ts_secs_set;
*swig_receipt_ts_seqno_get = *dtnapic::dtn_status_report_receipt_ts_seqno_get;
*swig_receipt_ts_seqno_set = *dtnapic::dtn_status_report_receipt_ts_seqno_set;
*swig_custody_ts_secs_get = *dtnapic::dtn_status_report_custody_ts_secs_get;
*swig_custody_ts_secs_set = *dtnapic::dtn_status_report_custody_ts_secs_set;
*swig_custody_ts_seqno_get = *dtnapic::dtn_status_report_custody_ts_seqno_get;
*swig_custody_ts_seqno_set = *dtnapic::dtn_status_report_custody_ts_seqno_set;
*swig_forwarding_ts_secs_get = *dtnapic::dtn_status_report_forwarding_ts_secs_get;
*swig_forwarding_ts_secs_set = *dtnapic::dtn_status_report_forwarding_ts_secs_set;
*swig_forwarding_ts_seqno_get = *dtnapic::dtn_status_report_forwarding_ts_seqno_get;
*swig_forwarding_ts_seqno_set = *dtnapic::dtn_status_report_forwarding_ts_seqno_set;
*swig_delivery_ts_secs_get = *dtnapic::dtn_status_report_delivery_ts_secs_get;
*swig_delivery_ts_secs_set = *dtnapic::dtn_status_report_delivery_ts_secs_set;
*swig_delivery_ts_seqno_get = *dtnapic::dtn_status_report_delivery_ts_seqno_get;
*swig_delivery_ts_seqno_set = *dtnapic::dtn_status_report_delivery_ts_seqno_set;
*swig_deletion_ts_secs_get = *dtnapic::dtn_status_report_deletion_ts_secs_get;
*swig_deletion_ts_secs_set = *dtnapic::dtn_status_report_deletion_ts_secs_set;
*swig_deletion_ts_seqno_get = *dtnapic::dtn_status_report_deletion_ts_seqno_get;
*swig_deletion_ts_seqno_set = *dtnapic::dtn_status_report_deletion_ts_seqno_set;
*swig_ack_by_app_ts_secs_get = *dtnapic::dtn_status_report_ack_by_app_ts_secs_get;
*swig_ack_by_app_ts_secs_set = *dtnapic::dtn_status_report_ack_by_app_ts_secs_set;
*swig_ack_by_app_ts_seqno_get = *dtnapic::dtn_status_report_ack_by_app_ts_seqno_get;
*swig_ack_by_app_ts_seqno_set = *dtnapic::dtn_status_report_ack_by_app_ts_seqno_set;
sub new {
    my $pkg = shift;
    my $self = dtnapic::new_dtn_status_report(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        dtnapic::delete_dtn_status_report($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : dtnapi::dtn_bundle ##############

package dtnapi::dtn_bundle;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( dtnapi );
%OWNER = ();
%ITERATORS = ();
*swig_source_get = *dtnapic::dtn_bundle_source_get;
*swig_source_set = *dtnapic::dtn_bundle_source_set;
*swig_dest_get = *dtnapic::dtn_bundle_dest_get;
*swig_dest_set = *dtnapic::dtn_bundle_dest_set;
*swig_replyto_get = *dtnapic::dtn_bundle_replyto_get;
*swig_replyto_set = *dtnapic::dtn_bundle_replyto_set;
*swig_priority_get = *dtnapic::dtn_bundle_priority_get;
*swig_priority_set = *dtnapic::dtn_bundle_priority_set;
*swig_dopts_get = *dtnapic::dtn_bundle_dopts_get;
*swig_dopts_set = *dtnapic::dtn_bundle_dopts_set;
*swig_expiration_get = *dtnapic::dtn_bundle_expiration_get;
*swig_expiration_set = *dtnapic::dtn_bundle_expiration_set;
*swig_creation_secs_get = *dtnapic::dtn_bundle_creation_secs_get;
*swig_creation_secs_set = *dtnapic::dtn_bundle_creation_secs_set;
*swig_creation_seqno_get = *dtnapic::dtn_bundle_creation_seqno_get;
*swig_creation_seqno_set = *dtnapic::dtn_bundle_creation_seqno_set;
*swig_delivery_regid_get = *dtnapic::dtn_bundle_delivery_regid_get;
*swig_delivery_regid_set = *dtnapic::dtn_bundle_delivery_regid_set;
*swig_sequence_id_get = *dtnapic::dtn_bundle_sequence_id_get;
*swig_sequence_id_set = *dtnapic::dtn_bundle_sequence_id_set;
*swig_obsoletes_id_get = *dtnapic::dtn_bundle_obsoletes_id_get;
*swig_obsoletes_id_set = *dtnapic::dtn_bundle_obsoletes_id_set;
*swig_payload_get = *dtnapic::dtn_bundle_payload_get;
*swig_payload_set = *dtnapic::dtn_bundle_payload_set;
*swig_status_report_get = *dtnapic::dtn_bundle_status_report_get;
*swig_status_report_set = *dtnapic::dtn_bundle_status_report_set;
sub new {
    my $pkg = shift;
    my $self = dtnapic::new_dtn_bundle(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        dtnapic::delete_dtn_bundle($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : dtnapi::dtn_session_info ##############

package dtnapi::dtn_session_info;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( dtnapi );
%OWNER = ();
%ITERATORS = ();
*swig_status_get = *dtnapic::dtn_session_info_status_get;
*swig_status_set = *dtnapic::dtn_session_info_status_set;
*swig_session_get = *dtnapic::dtn_session_info_session_get;
*swig_session_set = *dtnapic::dtn_session_info_session_set;
sub new {
    my $pkg = shift;
    my $self = dtnapic::new_dtn_session_info(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        dtnapic::delete_dtn_session_info($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


# ------- VARIABLE STUBS --------

package dtnapi;

*DTN_MAX_ENDPOINT_ID = *dtnapic::DTN_MAX_ENDPOINT_ID;
*DTN_MAX_AUTHDATA = *dtnapic::DTN_MAX_AUTHDATA;
*DTN_MAX_REGION_LEN = *dtnapic::DTN_MAX_REGION_LEN;
*DTN_MAX_BUNDLE_MEM = *dtnapic::DTN_MAX_BUNDLE_MEM;
*DTN_MAX_BLOCK_LEN = *dtnapic::DTN_MAX_BLOCK_LEN;
*DTN_MAX_BLOCKS = *dtnapic::DTN_MAX_BLOCKS;
*DTN_REGID_NONE = *dtnapic::DTN_REGID_NONE;
*DTN_REG_DROP = *dtnapic::DTN_REG_DROP;
*DTN_REG_DEFER = *dtnapic::DTN_REG_DEFER;
*DTN_REG_EXEC = *dtnapic::DTN_REG_EXEC;
*DTN_SESSION_CUSTODY = *dtnapic::DTN_SESSION_CUSTODY;
*DTN_SESSION_PUBLISH = *dtnapic::DTN_SESSION_PUBLISH;
*DTN_SESSION_SUBSCRIBE = *dtnapic::DTN_SESSION_SUBSCRIBE;
*COS_BULK = *dtnapic::COS_BULK;
*COS_NORMAL = *dtnapic::COS_NORMAL;
*COS_EXPEDITED = *dtnapic::COS_EXPEDITED;
*COS_RESERVED = *dtnapic::COS_RESERVED;
*DOPTS_NONE = *dtnapic::DOPTS_NONE;
*DOPTS_CUSTODY = *dtnapic::DOPTS_CUSTODY;
*DOPTS_DELIVERY_RCPT = *dtnapic::DOPTS_DELIVERY_RCPT;
*DOPTS_RECEIVE_RCPT = *dtnapic::DOPTS_RECEIVE_RCPT;
*DOPTS_FORWARD_RCPT = *dtnapic::DOPTS_FORWARD_RCPT;
*DOPTS_CUSTODY_RCPT = *dtnapic::DOPTS_CUSTODY_RCPT;
*DOPTS_DELETE_RCPT = *dtnapic::DOPTS_DELETE_RCPT;
*DOPTS_SINGLETON_DEST = *dtnapic::DOPTS_SINGLETON_DEST;
*DOPTS_MULTINODE_DEST = *dtnapic::DOPTS_MULTINODE_DEST;
*DOPTS_DO_NOT_FRAGMENT = *dtnapic::DOPTS_DO_NOT_FRAGMENT;
*BLOCK_FLAG_NONE = *dtnapic::BLOCK_FLAG_NONE;
*BLOCK_FLAG_REPLICATE = *dtnapic::BLOCK_FLAG_REPLICATE;
*BLOCK_FLAG_REPORT = *dtnapic::BLOCK_FLAG_REPORT;
*BLOCK_FLAG_DELETE_BUNDLE = *dtnapic::BLOCK_FLAG_DELETE_BUNDLE;
*BLOCK_FLAG_LAST = *dtnapic::BLOCK_FLAG_LAST;
*BLOCK_FLAG_DISCARD_BLOCK = *dtnapic::BLOCK_FLAG_DISCARD_BLOCK;
*BLOCK_FLAG_UNPROCESSED = *dtnapic::BLOCK_FLAG_UNPROCESSED;
*REASON_NO_ADDTL_INFO = *dtnapic::REASON_NO_ADDTL_INFO;
*REASON_LIFETIME_EXPIRED = *dtnapic::REASON_LIFETIME_EXPIRED;
*REASON_FORWARDED_UNIDIR_LINK = *dtnapic::REASON_FORWARDED_UNIDIR_LINK;
*REASON_TRANSMISSION_CANCELLED = *dtnapic::REASON_TRANSMISSION_CANCELLED;
*REASON_DEPLETED_STORAGE = *dtnapic::REASON_DEPLETED_STORAGE;
*REASON_ENDPOINT_ID_UNINTELLIGIBLE = *dtnapic::REASON_ENDPOINT_ID_UNINTELLIGIBLE;
*REASON_NO_ROUTE_TO_DEST = *dtnapic::REASON_NO_ROUTE_TO_DEST;
*REASON_NO_TIMELY_CONTACT = *dtnapic::REASON_NO_TIMELY_CONTACT;
*REASON_BLOCK_UNINTELLIGIBLE = *dtnapic::REASON_BLOCK_UNINTELLIGIBLE;
*STATUS_RECEIVED = *dtnapic::STATUS_RECEIVED;
*STATUS_CUSTODY_ACCEPTED = *dtnapic::STATUS_CUSTODY_ACCEPTED;
*STATUS_FORWARDED = *dtnapic::STATUS_FORWARDED;
*STATUS_DELIVERED = *dtnapic::STATUS_DELIVERED;
*STATUS_DELETED = *dtnapic::STATUS_DELETED;
*STATUS_ACKED_BY_APP = *dtnapic::STATUS_ACKED_BY_APP;
*DTN_PAYLOAD_FILE = *dtnapic::DTN_PAYLOAD_FILE;
*DTN_PAYLOAD_MEM = *dtnapic::DTN_PAYLOAD_MEM;
*DTN_PAYLOAD_TEMP_FILE = *dtnapic::DTN_PAYLOAD_TEMP_FILE;
*DTN_SUCCESS = *dtnapic::DTN_SUCCESS;
*DTN_ERRBASE = *dtnapic::DTN_ERRBASE;
*DTN_EINVAL = *dtnapic::DTN_EINVAL;
*DTN_EXDR = *dtnapic::DTN_EXDR;
*DTN_ECOMM = *dtnapic::DTN_ECOMM;
*DTN_ECONNECT = *dtnapic::DTN_ECONNECT;
*DTN_ETIMEOUT = *dtnapic::DTN_ETIMEOUT;
*DTN_ESIZE = *dtnapic::DTN_ESIZE;
*DTN_ENOTFOUND = *dtnapic::DTN_ENOTFOUND;
*DTN_EINTERNAL = *dtnapic::DTN_EINTERNAL;
*DTN_EINPOLL = *dtnapic::DTN_EINPOLL;
*DTN_EBUSY = *dtnapic::DTN_EBUSY;
*DTN_EVERSION = *dtnapic::DTN_EVERSION;
*DTN_EMSGTYPE = *dtnapic::DTN_EMSGTYPE;
*DTN_ENOSPACE = *dtnapic::DTN_ENOSPACE;
*DTN_ERRMAX = *dtnapic::DTN_ERRMAX;
*Handles = *dtnapic::Handles;
*HandleID = *dtnapic::HandleID;
1;
