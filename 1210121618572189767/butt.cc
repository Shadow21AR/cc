{{$data := (dbGet .Channel.ID "uno").Value}}
{{$embed := index .Message.Embeds 0 | structToSdict}}
{{$hex := sdict "R" 0xFF0000 "G" 0x00FF00 "B" 0x0000FF "Y" 0xFFFF00}}
{{if eq .StrippedID "join"}}
	{{if $data.users.HasKey (str .User.ID)}}
		{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" "You are already in the game" "Eph" true)}}
	{{else if eq (len $data.users) 10}}
		{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" "only 10 player can join the game" "Eph" true)}}
	{{else}}
		{{$data.users.Set (str .User.ID) (sdict "cards" cslice)}}
		{{$m := ""}}{{range $k, $v := $data.users}}{{$m = printf "%s<@%s>\n" $m $k}}{{end}}
		{{$embed.Set "fields" (cslice (sdict "name" "Player(s) joined" "value" (or $m "NIL")))}}
		{{editMessage nil .Message.ID (complexMessageEdit "embed" $embed)}}
		{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" "You have joined the game" "Eph" true)}}
	{{end}}
{{else if eq .StrippedID "leave"}}{{/*handle 0 users later*/}}
	{{if $data.users.HasKey (str .User.ID)}}
		{{$data.users.Del (str .User.ID)}}
		{{$m := ""}}{{range $k, $v := $data.users}}{{$m = printf "%s<@%s>\n" $m $k}}{{end}}
		{{$embed.Set "fields" (cslice (sdict "name" "Player(s) joined" "value" (or $m "NIL")))}}
		{{editMessage nil .Message.ID (complexMessageEdit "embed" $embed)}}
		{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" "You left the game" "Eph" true)}}
	{{else}}
		{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" "You are not in the game" "Eph" true)}}
	{{end}}
{{else if eq .StrippedID "start"}}
	{{if ne $data.owner .User.ID}}
		{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" (print "You cant start the game, only <@" $data.owner "> can start it.") "Eph" true)}}
	{{else if lt (len $data.users) 2}}
		{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" "You need 2 or more player to play this game." "Eph" true)}}
	{{else}}
		{{$deck := cslice}}
		{{range $s := (cslice "0" "1" "2" "3" "4" "5" "6" "7" "8" "9" "REVERSE" "SKIP" "PLUS2")}}
			{{- range $c := (cslice "R" "G" "B" "Y")}}
				{{- if eq $s "0"}}{{- $deck = $deck.Append (print $c $s)}}{{- else}}{{- $deck = $deck.AppendSlice (cslice (print $c $s) (print $c $s))}}{{- end}}
			{{- end -}}
		{{end}}
		{{range (cslice "WILD" "WILD4")}}
			{{- $deck = $deck.AppendSlice (cslice (print "A" .) (print "A" .) (print "A" .) (print "A" .))}}
		{{end}}
		{{$deck = shuffle $deck}}
		{{range $k, $v := $data.users}}
			{{$v.Set "cards" (slice $deck 0 7)}}
			{{$deck = slice $deck 7}}
		{{end}}
		{{$turns := cslice}}{{range $k, $v := $data.users}}{{$turns = $turns.Append $k}}{{end}}
		{{$deck = slice $deck 1}}
		{{$_ := execTemplate "disable" (sdict "msg" .Message "mID" .Message.ID)}}
		{{$data.Set "deck" $deck}}{{$data.Set "turns" $turns}}
		{{$discCard := sdict "in" (index $deck 0)}}{{$_ := execTemplate "getCard" $discCard}}
		{{$myCol := ""}}{{if eq $discCard.col "A"}}{{$myCol = (index (shuffle (cslice "R" "G" "B" "Y")) 0)}}{{end}}
		{{$data.Set "disc" (sdict "cards" (cslice (index $deck 0)) "col" (or $myCol $discCard.col ))}}
		{{$mID := execTemplate "genResponse" (sdict "token" .Interaction.Token "data" $data)}}
		{{$data.Set "mID" $mID}}
	{{end}}
{{else if eq .StrippedID "play"}}
	{{if ne (index $data.turns 0) (str .User.ID)}}
		{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" "it's not your turn." "Eph" true)}}{{return}}
	{{end}}
	{{$_ := execTemplate "playButt" (sdict "T" .Interaction.Token "data" $data "User" .User)}}
{{else if eq .StrippedID "uno"}}
	{{if ne (index $data.turns 0) (str .User.ID)}}
		{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" "it's not your turn." "Eph" true)}}{{return}}
	{{end}}
	{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" "it's doesn't work yet xD" "Eph" true)}}{{return}}
{{else if eq .StrippedID "callout"}}
	{{if ne (index $data.turns 0) (str .User.ID)}}
		{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" "it's not your turn." "Eph" true)}}{{return}}
	{{end}}
	{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" "it's doesn't work yet xD" "Eph" true)}}{{return}}
{{else if eq .StrippedID "hand"}}
	{{if not (in $data.turns (str .User.ID))}}
		{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" "you aren't in the game." "Eph" true)}}{{return}}
	{{end}}
	{{$out := ""}}
	{{$cards := sort ($data.users.Get (str .User.ID)).cards}}
	{{range $k, $v := $cards}}
		{{- $out = printf "%s<:8008:%s>" $out ($data.emojis.Get $v)}}
	{{end}}
	{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Emb" (cembed "title" "your cards" "description" (print "# " $out)) "Eph" true)}}
{{else if eq .StrippedID "help"}}
	{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Emb" (cembed "description" "Click `Hand` to see your cards\nClick `Pickup` to pick a card when u dont have any cards to play.\nClick `Callout` someone out for for penalty.") "Eph" true)}}
{{else if eq .StrippedID "pickup"}}
	{{if ne (str .User.ID) (index $data.turns 0)}}
		{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" "it's not your turn." "Eph" true)}}{{return}}
	{{end}}
	{{$uCards := ($data.users.Get (str .User.ID)).cards}}
	{{if le (len $data.deck) 1}}{{$data.Set "deck" ($data.deck.AppendSlice (slice $data.disc.cards 1 | shuffle))}}{{$data.disc.Set "cards" (cslice (index $data.disc.cards 0))}}{{end}}
	{{$pCard := index $data.deck 0}}{{$uCards = $uCards.Append $pCard}}
	{{$data.Set "deck" (slice $data.deck 1)}}
	{{($data.users.Get (str .User.ID)).Set "cards" $uCards}}
	{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" (printf "# Picked <:8008:%s>" ($data.emojis.Get $pCard)) "Eph" true)}}
	{{$_ := execTemplate "disable" (sdict "msg" .Message "mID" .Message.ID)}}
	{{$data.Set "turns" ((slice $data.turns 1).Append (index $data.turns 0))}}
	{{$_ := execTemplate "genResponse" (sdict "token" .Interaction.Token "data" $data)}}
{{else if reFind `pick_` .StrippedID}}
	{{$args := split .StrippedID "_"}}
	{{if ne (index $args 1) (str .User.ID)}}
		{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" "it's not your card." "Eph" true)}}{{return}}
	{{end}}
	{{if eq (index $args 2) "cards"}}
		{{if ne (index $data.turns 0) (str .User.ID)}}{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" "it's not your turn." "Eph" true)}}{{return}}{{end}}
		{{$picked := index .Values 0}}
		{{$card := sdict "in" (index (split $picked "_") 0)}}{{$_ := execTemplate "getCard" $card}}
		{{if eq $card.in "none"}}
			{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" "Select a valid card or click `Pickup` button if you dont have any cards" "Eph" true)}}{{return}}
		{{end}}
		{{if not (in ($data.users.Get (str .User.ID)).cards $card.in)}}
			{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" "Select a valid card, you dont have that card" "Eph" true)}}{{return}}
		{{end}}
		{{$data.disc.Set "col" $card.col}}
		{{$uCards := cslice}}{{$check := $card.in}}
		{{range ($data.users.Get (str .User.ID)).cards}}{{- if eq $check .}}{{- $check = "8008"}}{{- else}}{{- $uCards = $uCards.Append .}}{{- end}}{{end}}
		{{($data.users.Get (str .User.ID)).Set "cards" $uCards}}
		{{$newDisc := (cslice $card.in).AppendSlice $data.disc.cards}}{{$data.disc.Set "cards" $newDisc}}
		{{if eq "WILD" $card.name}}
			{{if ne (index $data.turns 0) (str .User.ID)}}{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" "it's not your turn." "Eph" true)}}{{return}}{{end}}
			{{$opts := cslice}}{{range (cslice "🔴" "🟢" "🔵" "🟡")}}{{$opts = $opts.Append (sdict "label" . "value" .)}}{{end}}
			{{updateMessage (complexMessageEdit "embed" (cembed "description" (print .User.Mention " | Select the new color")) "components" (cmenu "custom_id" (print "uno_pick_" .User.ID "_col") "type" "text" "placeholder" "pick a card" "options" $opts))}}
		{{else if eq "WILD4" $card.name}}
			{{if ne (index $data.turns 0) (str .User.ID)}}{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" "it's not your turn." "Eph" true)}}{{return}}{{end}}
			{{$opts := cslice}}{{range (cslice "🔴" "🟢" "🔵" "🟡")}}{{$opts = $opts.Append (sdict "label" . "value" .)}}{{end}}
			{{updateMessage (complexMessageEdit "embed" (cembed "description" (print .User.Mention " | Select the new color")) "components" (cmenu "custom_id" (print "uno_pick_" .User.ID "_col") "type" "text" "placeholder" "pick a card" "options" $opts))}}
			{{$nUser := index $data.turns 0}}
			{{$data.Set "turns" ((slice $data.turns 1).Append (index $data.turns 0))}}
			{{$nUserCards := ($data.users.Get $nUser).cards}}
			{{if le (len $data.deck) 4}}{{$data.Set "deck" ($data.deck.AppendSlice (slice $data.disc.cards 1 | shuffle))}}{{$data.disc.Set "cards" (cslice (index $data.disc.cards 0))}}{{end}}
			{{$nUserCards =$nUserCards.AppendSlice (slice $data.deck 0 4)}}{{($data.users.Get $nUser).Set "cards" $nUserCards}}{{$data.Set "deck" (slice $data.deck 4)}}
			{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Emb" (cembed "description" (print "Added 4 cards to <@" $nUser ">'s hand.")))}}
		{{else if reFind `REVERSE` $card.name}}
			{{if ne (index $data.turns 0) (str .User.ID)}}{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" "it's not your turn." "Eph" true)}}{{return}}{{end}}
			{{$newTurn := cslice}}{{range $data.turns}}{{- $newTurn = (cslice .).AppendSlice $newTurn}}{{end}}{{$data.Set "turns" $newTurn}}
			{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" "The direction of play has been reversed!")}}
			{{$_ := execTemplate "genResponse" (sdict "token" .Interaction.Token "data" $data)}}
		{{else if reFind `SKIP` $card.name}}
			{{if ne (index $data.turns 0) (str .User.ID)}}{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" "it's not your turn." "Eph" true)}}{{return}}{{end}}
			{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" (printf "<@%d> is skipping the <@%s>'s turn!" .User.ID (index $data.turns 1)))}}
			{{$data.Set "turns" ((slice $data.turns 1).Append (index $data.turns 0))}}
			{{$_ := execTemplate "genResponse" (sdict "token" .Interaction.Token "data" $data)}}
		{{else if reFind `PLUS2` $card.name}}
			{{if ne (index $data.turns 0) (str .User.ID)}}{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" "it's not your turn." "Eph" true)}}{{return}}{{end}}
			{{$nUser := index $data.turns 0}}{{$nUserCards := ($data.users.Get $nUser).cards}}
			{{if le (len $data.deck) 2}}{{$data.Set "deck" ($data.deck.AppendSlice (slice $data.disc.cards 1 | shuffle))}}{{$data.disc.Set "cards" (cslice (index $data.disc.cards 0))}}{{end}}
			{{$nUserCards = $nUserCards.AppendSlice (slice $data.deck 0 2)}}{{($data.users.Get $nUser).Set "cards" $nUserCards}}{{$data.Set "deck" (slice $data.deck 2)}}
			{{$_ := execTemplate "sendResponse" (sdict "T" .Interaction.Token "Con" (printf "<@%d> played Draw 2 card, <@%s> drew 2 cards and skipped their turn.\n" .User.ID (index $data.turns 0)))}}
			{{$data.Set "turns" ((slice $data.turns 1).Append (index $data.turns 0))}}
			{{$_ := execTemplate "genResponse" (sdict "token" .Interaction.Token "data" $data)}}
		{{else}}
			{{ $data.Set "turns" ((slice $data.turns 1).Append (index $data.turns 0)) }}
			{{$_ := execTemplate "genResponse" (sdict "token" .Interaction.Token "data" $data)}}
		{{end}}
	{{else if eq (index $args 2) "col"}}
		{{$data.disc.Set "col" ((sdict "🔴" "R" "🟢" "G" "🔵" "B" "🟡" "Y").Get (index .Values 0))}}
		{{$mID := execTemplate "genResponse" (sdict "token" .Interaction.Token "data" $data)}}
		{{$data.Set "mID" $mID}}
	{{end}}
	{{if le (len $uCards) 0}}
		{{$winner := .User}}
		{{$score := or ($data.users.Get (str .User.ID)).score 0}}
		{{range $data.users}}
			{{- range $cards}}
				{{- $face := slice . 1}}
				{{- if eq $face "0" "1" "2" "3" "4" "5" "6" "7" "8" "9"}}
					{{- $score = add 3 $score}}
				{{- else if eq $face "REVERSE" "SKIP" "PLUS2"}}
					{{- $score = add 20 $score}}
				{{- else if eq $face "WILD" "WILD4"}}
					{{- $score = add 50 $score}}
				{{- end}}
			{{- end}}
			{{.Set "cards" cslice}}
		{{end}}
		{{($data.users.Get (str .User.ID)).Set "score" $score}}
		{{range $k, $v := $data.users}}{{/*TOo lazy to test, works in my mind xD*/}}
			{{- if ge $v.score 500}}
				{{sendMessageRetID nil (print "<@" $k "> WON!")}}
				{{return}}
			{{- end}}
		{{end}}
		{{$deck := cslice}}{{/*Should restart the game ig*/}}
		{{range $s := (cslice "0" "1" "2" "3" "4" "5" "6" "7" "8" "9" "REVERSE" "SKIP" "PLUS2")}}
			{{- range $c := (cslice "R" "G" "B" "Y")}}
				{{- if eq $s "0"}}{{- $deck = $deck.Append (print $c $s)}}{{- else}}{{- $deck = $deck.AppendSlice (cslice (print $c $s) (print $c $s))}}{{- end}}
			{{- end -}}
		{{end}}
		{{range (cslice "WILD" "WILD4")}}
			{{- $deck = $deck.AppendSlice (cslice (print "A" .) (print "A" .) (print "A" .) (print "A" .))}}
		{{end}}
		{{$deck = shuffle $deck}}
		{{range $k, $v := $data.users}}
			{{$v.Set "cards" (slice $deck 0 7)}}
			{{$deck = slice $deck 7}}
		{{end}}
		{{$turns := cslice}}{{range $k, $v := $data.users}}{{$turns = $turns.Append $k}}{{end}}
		{{$deck = slice $deck 1}}
		{{$_ := execTemplate "disable" (sdict "msg" .Message "mID" .Message.ID)}}
		{{$data.Set "deck" $deck}}{{$data.Set "turns" $turns}}
		{{$discCard := sdict "in" (index $deck 0)}}{{$_ := execTemplate "getCard" $discCard}}
		{{$myCol := ""}}{{if eq $discCard.col "A"}}{{$myCol = (index (shuffle (cslice "R" "G" "B" "Y")) 0)}}{{end}}
		{{$data.Set "disc" (sdict "cards" (cslice (index $deck 0)) "col" (or $myCol $discCard.col ))}}
		{{$mID := execTemplate "genResponse" (sdict "token" .Interaction.Token "data" $data)}}
		{{$data.Set "mID" $mID}}
	{{end}}
{{end}}

{{define "sendResponse"}}
	{{$out := sdict}}
	{{if .Con}}{{$out.Set "content" .Con}}{{end}}{{if .Emb}}{{$out.Set "embed" .Emb}}{{end}}{{if .Com}}{{$out.Set "components" .Com}}{{end}}{{if .Eph}}{{$out.Set "ephemeral" true}}{{end}}
	{{if .RID}}{{sendResponseRetID .T (complexMessage $out)}}{{else}}{{sendResponse .T (complexMessage $out)}}{{end}}
{{end}}

{{define "genResponse"}}
	{{$hex := sdict "R" 0xFF0000 "G" 0x00FF00 "B" 0x0000FF "Y" 0xFFFF00}}{{$token := .token}}
	{{$menu := sdict "data" .data "user" (index .data.turns 0)}}{{$_ := execTemplate "CMenu" $menu}}{{$data := .data}}
	{{$cards := (.data.users.Get $menu.user).cards}}
	{{$uno := cslice}}{{if le (len $cards) 2}}{{$uno := cslice (cbutton "label" "Uno" "custom_id" "uno_uno" "style" "Success")}}{{end}}
	{{$_ := execTemplate "sendResponse" (sdict "T" $token "Emb" (cembed "color" ($hex.Get $data.disc.col) "description" (printf "The current card is: \n# <:e:%s>\n\nIt's <@%s> turn." ($data.emojis.Get (index $data.disc.cards 0)) (index $data.turns 0))) "Com" (cslice (cbutton "label" "Hand" "custom_id" "uno_hand" "style" "Success") (cbutton "label" "Play" "custom_id" "uno_play" "style" "Success" "disabled" (not $menu.pickup)) (cbutton "label" "Pickup" "custom_id" "uno_pickup" "style" "Success" "disabled" $menu.pickup) (cbutton "label" "Callout" "custom_id" "uno_callout" "style" "Danger") (cbutton "label" "Help" "custom_id" "uno_help" "style" "Primary")).AppendSlice $uno)}}
{{end}}

{{define "playButt"}}
	{{$hex := sdict "R" 0xFF0000 "G" 0x00FF00 "B" 0x0000FF "Y" 0xFFFF00}}
	{{$menu := sdict "data" .data "user" (index .data.turns 0)}}{{$_ := execTemplate "CMenu" $menu}}{{$data := .data}}
	{{$_ := execTemplate "sendResponse" (sdict "T" .T "Emb" (cembed "color" ($hex.Get $data.disc.col) "description" "Pick a card!") "Com" $menu.out "Eph" true)}}
{{end}}

{{define "CMenu"}}
	{{$cards := (.data.users.Get .user).cards | sort}}{{$data := .data}}
	{{if not $cards}}{{return}}{{end}}{{$options := cslice}}{{$out := cslice}}{{$i := 1}}
	{{range $cards}}
		{{$card := sdict "in" . "name" .}}{{$_ := execTemplate "getCard" $card}}
		{{$discCard := sdict "in" (index $data.disc.cards 0) "name" .}}{{$_ := execTemplate "getCard" $discCard}}
		{{if or (eq $card.col $data.disc.col) (eq $card.val $discCard.val) (inFold $card.name "WILD")}}
			{{$options = $options.Append (sdict "label" $card.name "value" (print . "_" (randInt 999999999)) "emoji" (sdict "id" ($data.emojis.Get .)))}}
		{{end}}
	{{end}}
	{{while $options}}{{.Set "pickup" true}}
		{{$max := 24}}{{if gt $max (len $options)}}{{$max = len $options}}{{end}}
		{{$out = $out.Append (cmenu "custom_id" (print "uno_pick_" .user "_cards_" $i) "type" "text" "placeholder" "pick a card" "options" ((cslice (sdict "label" "None" "value" (print "none" "_" "none"))).AppendSlice (slice $options 0 $max)))}}
		{{if gt (len $options) 24}}{{$i = add $i 1}}{{$options = slice $options $max}}{{else}}{{break}}{{end}}
	{{end}}
	{{.Set "out" $out}}
{{end}}

{{define "getCard"}}
	{{$cols := sdict "A" "" "R" "Red" "B" "Blue" "G" "Green" "Y" "Yellow"}}
	{{.Set "col" (slice .in 0 1)}}{{.Set "val" (slice .in 1)}}
	{{.Set "name" (joinStr " " ($cols.Get .col) .val)}}
{{end}}

{{define "disable"}}
	{{$msg := .msg}}
	{{$comp := $msg.Components}}{{$newComp := cslice }}
	{{range $k, $v := $comp}}{{$row := cslice}}
		{{- range $m, $n := $v.Components}}
			{{- $n := structToSdict $n}}{{- $n.Set "Disabled" true}}
			{{- if $n.Style }}{{- $n = cbutton $n}}
			{{- else}}{{- $n = cmenu $n}}{{- end}}
			{{- $row = $row.Append $n}}
		{{- end}}
		{{- $newComp = $newComp.Append $row}}
	{{end}}
	{{editMessage nil .mID (complexMessageEdit "components" $newComp)}}
{{end}}

{{dbSet .Channel.ID "uno" $data}}