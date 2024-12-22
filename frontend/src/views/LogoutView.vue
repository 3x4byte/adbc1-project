<template>
</template>

<script>
export default{
	async mounted(){
      await this.$pb.authStore.clear()
      this.$router.push('/login'); // Weiterleiten
	}
}
</script>